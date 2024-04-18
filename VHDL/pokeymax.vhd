---------------------------------------------------------------------------
-- (c) 2020 mark watson
-- I am happy for anyone to use this for non-commercial use.
-- If my vhdl files are used commercially or otherwise sold,
-- please contact me for explicit permission at scrameta (gmail).
-- This applies for source and binary form and derived works.
---------------------------------------------------------------------------

LIBRARY ieee;
USE ieee.std_logic_1164.all; 
use ieee.numeric_std.all;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
use IEEE.STD_LOGIC_MISC.all;
use work.AudioTypes.all;

LIBRARY work;

ENTITY pokeymax IS 
	GENERIC
	(
		pokeys : integer := 1; -- 1-4
		lowpass : integer := 0; -- 0=lowpass off, 1=lowpass on (leave on except if there is no space! Low impact...)
		enable_auto_stereo : integer := 0;   -- 1=auto detect a4 => not toggling => mono

		fancy_switch_bit : integer := 20; -- 0=ext is low => mono
		gtia_audio_bit : integer := 0;    -- 0=no gtia on l/r,1=gtia mixed on l/r
		detect_right_on_by_default : integer := 1; 
		saturate_on_by_default : integer := 1; 
		a4_bit : integer := 0;
		a5_bit : integer := 0;
		a6_bit : integer := 0;
		a7_bit : integer := 0;
		cs0_bit : integer := 18;
		cs1_bit : integer := 19;
		spdif_bit : integer := 0;
		ps2clk_bit : integer := 0;
		ps2dat_bit : integer := 0;

		ext_bits : integer := 3; 
		pll_v2 : integer := 1;

		enable_config : integer := 1;
		enable_sid : integer := 0;
		enable_psg : integer := 0;
		enable_covox : integer := 0;
		enable_sample : integer := 0;
		enable_flash : integer := 0;
		enable_audout2: integer := 1;
		enable_spdif: integer := 0;
		enable_ps2: integer := 0;
	
		sid_wave_base : integer := 42496; --to_integer(unsigned(x"a600"));

		flash_addr_bits : integer := 16;

		ext_clk_enable : integer := 0; -- Use PADDLE(6) for sid clk enable, PADDLE(7) for psg

   		version : STRING  := "DEVELOPR" -- 8 char string atascii
	);
	PORT
	(
		PHI2 : IN STD_LOGIC;
		
		CLK_OUT : OUT STD_LOGIC; -- Use PHI2 and internal oscillator to create a clock, feed out here
		CLK_SLOW : IN STD_LOGIC; -- ... and back in here, then to pll!		

		CLK0 : IN STD_LOGIC; -- 50MHz on v3 only
		CLK1 : IN STD_LOGIC; -- 50MHz on v3 only
		
		D :  INOUT  STD_LOGIC_VECTOR(7 DOWNTO 0);
		A :  IN  STD_LOGIC_VECTOR(3 DOWNTO 0);
		W_N : IN STD_LOGIC;
		IRQ : INOUT STD_LOGIC;
		SOD : OUT STD_LOGIC;
		ACLK : OUT STD_LOGIC;
		BCLK : INOUT STD_LOGIC;
		SID : IN STD_LOGIC;
		CS0_N : IN STD_LOGIC;
		CS1 : IN STD_LOGIC;

		AUD : OUT STD_LOGIC_VECTOR(4 DOWNTO 1);

		EXT : INOUT STD_LOGIC_VECTOR(EXT_BITS DOWNTO 1);

		PADDLE : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		POTRESET_N : OUT STD_LOGIC;

		IOX_RST : OUT STD_LOGIC;
		IOX_INT : IN STD_LOGIC;
		IOX_SDA : INOUT STD_LOGIC;
		IOX_SCL : INOUT STD_LOGIC
	);
END pokeymax;		
		
ARCHITECTURE vhdl OF pokeymax IS
	component int_osc is
	port (
		clkout : out std_logic;        -- clkout.clk
		oscena : in  std_logic := '0'  -- oscena.oscena
	);
	end component;

	component pll
		port (
			inclk0   : in  std_logic := '0';
			c0 : out std_logic;
			c1 : out std_logic;
			c2 : out std_logic;
			locked   : out std_logic
		);
	end component;

	component pllv3
		port (
			inclk0   : in  std_logic := '0';
			c0 : out std_logic;
			c1 : out std_logic;
			c2 : out std_logic;
			c3 : out std_logic;
			locked   : out std_logic
		);
	end component;

	signal OSC_CLK : std_logic; -- about 82MHz! Always?? Massive range on data sheet

	signal CLK : std_logic;
	signal CLK116 : std_logic;
	signal CLK106 : std_logic;
	signal RESET_N : std_logic;

	signal ENABLE_CYCLE : std_logic;
	signal ENABLE_DOUBLE_CYCLE : std_logic;

	-- WRITE ENABLES
	SIGNAL POKEY_WRITE_ENABLE : STD_LOGIC_VECTOR(3 downto 0);		
	
	SIGNAL SID_WRITE_ENABLE : STD_LOGIC_VECTOR(1 downto 0);	
	SIGNAL SID_READ_ENABLE : STD_LOGIC_VECTOR(1 downto 0);	

	SIGNAL PSG_WRITE_ENABLE : STD_LOGIC_VECTOR(1 downto 0);	

	SIGNAL SAMPLE_WRITE_ENABLE : STD_LOGIC;	
	SIGNAL CONFIG_WRITE_ENABLE : STD_LOGIC;	
	
	-- DATA OUTS
	type DO_TYPE is array (NATURAL range <>) of std_logic_vector(7 downto 0);
	
	SIGNAL POKEY_DO : DO_TYPE(3 downto 0);	
	
	SIGNAL SID_DO : DO_TYPE(1 downto 0);
	SIGNAL SID_DRIVE_DO : std_logic_vector(1 downto 0);
	
	SIGNAL PSG_DO : DO_TYPE(1 DOWNTO 0);	
	
	SIGNAL SAMPLE_DO : STD_LOGIC_VECTOR(7 DOWNTO 0);	
	SIGNAL CONFIG_DO : STD_LOGIC_VECTOR(7 DOWNTO 0);	
	
	-- POKEY	
	signal POKEY_CHANNEL0 : POKEY_AUDIO(3 downto 0);
	signal POKEY_CHANNEL1 : POKEY_AUDIO(3 downto 0);
	signal POKEY_CHANNEL2 : POKEY_AUDIO(3 downto 0);
	signal POKEY_CHANNEL3 : POKEY_AUDIO(3 downto 0);

	signal CHANNEL0SUM_NEXT : unsigned(5 downto 0);	
	signal CHANNEL1SUM_NEXT : unsigned(5 downto 0);
	signal CHANNEL2SUM_NEXT : unsigned(5 downto 0);
	signal CHANNEL3SUM_NEXT : unsigned(5 downto 0);
	signal CHANNEL0SUM_REG : unsigned(5 downto 0);	
	signal CHANNEL1SUM_REG : unsigned(5 downto 0);
	signal CHANNEL2SUM_REG : unsigned(5 downto 0);
	signal CHANNEL3SUM_REG : unsigned(5 downto 0);	
	
	signal SIO_CLOCKIN_IN : std_logic;
	signal SIO_CLOCKIN_OUT : std_logic;
	signal SIO_CLOCKIN_OE : std_logic;
	signal SIO_CLOCKOUT : std_logic;

	signal SIO_TXD : std_logic;
	signal SIO_RXD : std_logic;
	signal SIO_RXD_SYNC : std_logic;

	signal POKEY_IRQ : std_logic_vector(3 downto 0);

	signal ADDR_IN : std_logic_vector(7 downto 0);
	signal WRITE_DATA : std_logic_vector(7 downto 0);
	signal DEVICE_ADDR : std_logic_vector(3 downto 0);

	signal POKEY_AUDIO_0 : unsigned(15 downto 0);
	signal POKEY_AUDIO_1 : unsigned(15 downto 0);
	signal POKEY_AUDIO_2 : unsigned(15 downto 0);
	signal POKEY_AUDIO_3 : unsigned(15 downto 0);
	
	signal AUDIO_0_UNSIGNED : unsigned(15 downto 0);
	signal AUDIO_1_UNSIGNED : unsigned(15 downto 0);
	signal AUDIO_2_UNSIGNED : unsigned(15 downto 0);
	signal AUDIO_3_UNSIGNED : unsigned(15 downto 0);	
	
	signal AUDIO_0_SIGMADELTA : std_logic;
	signal AUDIO_1_SIGMADELTA : std_logic;
	signal AUDIO_2_SIGMADELTA : std_logic;
	signal AUDIO_3_SIGMADELTA : std_logic;

	signal KEYBOARD_SCAN : std_logic_vector(5 downto 0);
	signal IOX_KEYBOARD_RESPONSE : std_logic_vector(1 downto 0);
	signal PS2_KEYBOARD_RESPONSE : std_logic_vector(1 downto 0);
	signal KEYBOARD_RESPONSE : std_logic_vector(1 downto 0);
	signal KEYBOARD_SCAN_UPDATE : std_logic;
	signal KEYBOARD_SCAN_ENABLE : std_logic;

	signal POKEY_PROFILE_ADDR : std_logic_vector(5 downto 0);
	signal POKEY_PROFILE_REQUEST : std_logic;
	signal POKEY_PROFILE_READY : std_logic;

	-- SID
	signal SID_CLK_ENABLE : std_logic;
	signal SID_AUDIO : SID_AUDIO_TYPE(1 downto 0);
	signal SID_FLASH1_ADDR : std_logic_vector(16 downto 0);
        signal SID_FLASH1_ROMREQUEST : std_logic;
        signal SID_FLASH1_ROMREADY : std_logic;
	signal SID_FLASH2_ADDR : std_logic_vector(16 downto 0);
        signal SID_FLASH2_ROMREQUEST : std_logic;
        signal SID_FLASH2_ROMREADY : std_logic;
	signal SID_FILTER1_REG : std_logic_vector(1 downto 0);
	signal SID_FILTER1_NEXT : std_logic_vector(1 downto 0);
	signal SID_FILTER2_REG : std_logic_vector(1 downto 0);
	signal SID_FILTER2_NEXT : std_logic_vector(1 downto 0);
	signal SID1_FILTER_BP : signed(17 downto 8);
	signal SID1_FILTER_HP : signed(17 downto 8);
	signal SID1_F_RAW : std_logic_vector(12 downto 0);
	signal SID1_F_BP : unsigned(12 downto 0);
	signal SID1_F_HP : unsigned(12 downto 0);
	signal SID2_FILTER_BP : signed(17 downto 8);
	signal SID2_FILTER_HP : signed(17 downto 8);
	signal SID2_F_RAW : std_logic_vector(12 downto 0);
	signal SID2_F_BP : unsigned(12 downto 0);
	signal SID2_F_HP : unsigned(12 downto 0);
	
	-- PSG
	signal PSG_ENABLE_2Mhz : std_logic;
	signal PSG_ENABLE_1Mhz : std_logic;
	signal PSG_ENABLE : std_logic;
	signal PSG_AUDIO : PSG_AUDIO_TYPE(1 downto 0);	

	signal PSG_CHANNEL : PSG_CHANNEL_TYPE(5 downto 0);	
	signal PSG_CHANGED : std_logic_vector(1 downto 0);

	signal PSG_FREQ_REG : std_logic_vector(1 downto 0);
	signal PSG_FREQ_NEXT : std_logic_vector(1 downto 0);

	signal PSG_STEREOMODE_REG : std_logic_vector(1 downto 0);
	signal PSG_STEREOMODE_NEXT : std_logic_vector(1 downto 0);

	signal PSG_PROFILESEL_REG : std_logic_vector(1 downto 0);
	signal PSG_PROFILESEL_NEXT : std_logic_vector(1 downto 0);
	signal PSG_PROFILE_ADDR : std_logic_vector(4 downto 0);
	signal PSG_PROFILE_REQUEST : std_logic;
	signal PSG_PROFILE_READY : std_logic;

	signal PSG_ENVELOPE16_REG : std_logic;
	signal PSG_ENVELOPE16_NEXT : std_logic;

	signal PSG_MIX1 : std_logic_vector(5 downto 0);
	signal PSG_MIX2 : std_logic_vector(5 downto 0);
	
	-- SUPPORT	
	signal BUS_DATA : std_logic_vector(7 downto 0);
	signal BUS_OE : std_logic;

	signal REQUEST : std_logic;
	signal WRITE_N : std_logic;

	signal DO_MUX : std_logic_vector(7 downto 0);
	signal DRIVE_DO_MUX : std_logic;

	signal i2c0_ena : std_logic;
	signal i2c0_addr : std_logic_vector(7 downto 1);
	signal i2c0_rw : std_logic;
	signal i2c0_write_data : std_logic_vector(7 downto 0);
	signal i2c0_busy : std_logic;
	signal i2c0_read_data : std_logic_vector(7 downto 0);
	signal i2c0_error : std_logic;

	signal CS0NMOD : std_logic;
	signal CS1MOD : std_logic;
	signal CS_COMB : std_logic;

	signal AIN : std_logic_vector(7 downto 0);

	signal POTRESET : std_logic;

	signal FANCY_ENABLE : std_logic;
	signal FANCY_SWITCH : std_logic;
	signal A4_DETECTED : std_logic;
	signal GTIA_AUDIO : std_logic;

	signal EXT_INT : std_logic_vector(20 downto 0);

	-- config
		--config regs
	signal DETECT_RIGHT_REG : std_logic;
	signal IRQ_EN_REG : std_logic;
	signal CHANNEL_MODE_REG : std_logic;
	signal SATURATE_REG : std_logic;
	signal POST_DIVIDE_REG : std_logic_vector(7 downto 0);	
	signal GTIA_ENABLE_REG : std_logic_vector(3 downto 0);
	signal VERSION_LOC_REG : std_logic_vector(2 downto 0);
	signal PAL_REG : std_logic;
	
	signal DETECT_RIGHT_NEXT : std_logic;
	signal IRQ_EN_NEXT : std_logic;
	signal CHANNEL_MODE_NEXT : std_logic;
	signal SATURATE_NEXT : std_logic;
	signal POST_DIVIDE_NEXT : std_logic_vector(7 downto 0);
	signal GTIA_ENABLE_NEXT : std_logic_vector(3 downto 0);
	signal VERSION_LOC_NEXT : std_logic_vector(2 downto 0);
	signal PAL_NEXT : std_logic;
	
		--config infra
	signal addr_decoded4 : std_logic_vector(15 downto 0);	
	signal CONFIG_ENABLE_REG : std_logic;
	signal CONFIG_ENABLE_NEXT: std_logic;
	
	-- SAMPLE/COVOX
	signal SAMPLE_AUDIO : SAMPLE_AUDIO_TYPE(1 downto 0);
	signal SAMPLE_IRQ : std_logic;
	signal SAMPLE_RAM_ADDRESS : std_logic_vector(15 downto 0);
	signal SAMPLE_RAM_WRITE_ENABLE : std_logic;
	signal SAMPLE_RAM_DATA : std_logic_vector(7 downto 0);

	signal ADPCM_STEP_ADDR : std_logic_vector(6 downto 0);
	signal ADPCM_STEP_REQUEST : std_logic;
	signal ADPCM_STEP_READY : std_logic;

	-- FLASH
	signal flash_do_slow : std_logic_vector(31 downto 0); --58Mhz

	signal CPU_FLASH_REQUEST_NEXT : std_logic;
	signal CPU_FLASH_REQUEST_REG : std_logic;
	signal CPU_FLASH_WRITE_N_NEXT : std_logic;
	signal CPU_FLASH_WRITE_N_REG : std_logic;
	signal CPU_FLASH_CFG_NEXT : std_logic;
	signal CPU_FLASH_CFG_REG : std_logic;
	signal CPU_FLASH_ADDR_NEXT : std_logic_vector(flash_addr_bits+1 downto 0);
	signal CPU_FLASH_ADDR_REG : std_logic_vector(flash_addr_bits+1 downto 0);
	signal CPU_FLASH_DATA_NEXT : std_logic_vector(31 downto 0);
	signal CPU_FLASH_DATA_REG : std_logic_vector(31 downto 0);
	signal CPU_FLASH_COMPLETE : std_logic;

	signal CONFIG_FLASH_STATE_REG : std_logic_vector(1 downto 0);
	signal CONFIG_FLASH_STATE_NEXT : std_logic_vector(1 downto 0);
	constant CONFIG_FLASH_STATE_ADDR1 : std_logic_vector(1 downto 0) := "00";
	constant CONFIG_FLASH_STATE_ADDR2 : std_logic_vector(1 downto 0) := "01";
	constant CONFIG_FLASH_STATE_DONE : std_logic_vector(1 downto 0) := "10";
	signal CONFIG_FLASH_REQUEST : std_logic;
	signal CONFIG_FLASH_COMPLETE : std_logic;
	signal CONFIG_FLASH_ADDR : std_logic_vector(0 downto 0);

	-- capability restriction
	signal RESTRICT_CAPABILITY_REG : std_logic_vector(4 downto 0);
	signal RESTRICT_CAPABILITY_NEXT : std_logic_vector(4 downto 0);
	signal readreq_s : std_logic;
	signal writereq_s : std_logic;
	-- 0=stereo off
	-- 1=quad off
	-- 2=sid off
	-- 3=psg off
	-- 4=sample off

	-- output channel on/off
	signal CHANNEL_EN_REG : std_logic_vector(4 downto 0);
	signal CHANNEL_EN_NEXT : std_logic_vector(4 downto 0);
	-- 0=0 (37)
	-- 1=1
	-- 2=2 L ext 
	-- 3=3 R ext
	-- 4=spdif

	-- ext clk enable
	signal SID_ENABLE_NEXT : std_logic;
	signal SID_ENABLE_REG : std_logic;
	signal PSG_ENABLE_NEXT : std_logic;
	signal PSG_ENABLE_REG : std_logic;

	-- clock gen
	signal MHZ1_ENABLE : std_logic;
	signal MHZ2_ENABLE : std_logic;

	-- spdif
	signal spdif_mux : std_logic_vector(15 downto 0);
	signal spdif_right : std_logic;
	signal spdif_out : std_logic;
	signal CLK6144 : std_logic; --spdif
	signal AUDIO_2_FILTERED : unsigned(15 downto 0);
	signal AUDIO_3_FILTERED : unsigned(15 downto 0);	

	-- ps2
	signal PS2CLK : std_logic;
	signal PS2DAT : std_logic;

	function getByte(a : string; x : integer) return std_logic_vector is
   		 variable ret : std_logic_vector(7 downto 0);
	begin
	        ret := std_logic_vector(to_unsigned(character'pos(a(x)), 8));
	    return ret;
	end function getByte;

	function MIN(LEFT, RIGHT: INTEGER) return INTEGER is
	begin
	  if LEFT < RIGHT then return LEFT;
	  else return RIGHT;
	  end if;
	end function min;
	
BEGIN
	IOX_RST <= 'Z'; -- TODO weak pull up in pins (see TODO file)
	EXT <= (others=>'Z');

	CS_COMB <= CS1MOD and not(CS0NMOD);

	oscillator : int_osc
	port map 
	(
		clkout => OSC_CLK, 
		oscena => '1'
	);

flash_on : if enable_flash=1 generate 

	process(CLK,RESET_N)
	begin
		if (RESET_N='0') then
			CPU_FLASH_REQUEST_REG <= '0';
			CPU_FLASH_WRITE_N_REG <= '1';
			CPU_FLASH_CFG_REG <= '0';
			CPU_FLASH_ADDR_REG <= (others=>'0');
			CPU_FLASH_DATA_REG <= (others=>'0');
			CONFIG_FLASH_STATE_REG <= CONFIG_FLASH_STATE_ADDR1;
		elsif (CLK'event and CLK='1') then
			CPU_FLASH_REQUEST_REG <= CPU_FLASH_REQUEST_NEXT;
			CPU_FLASH_WRITE_N_REG <= CPU_FLASH_WRITE_N_NEXT;
			CPU_FLASH_CFG_REG <= CPU_FLASH_CFG_NEXT;
			CPU_FLASH_ADDR_REG <= CPU_FLASH_ADDR_NEXT;
			CPU_FLASH_DATA_REG <= CPU_FLASH_DATA_NEXT;
			CONFIG_FLASH_STATE_REG <= CONFIG_FLASH_STATE_NEXT;
		end if;
	end process;

	-- TODO:req2 initially reads some data for the config regs
	-- then the state machine fills the entirely of block ram
	-- say it takes 10 cycles for 32-bits, this will take... 0.7ms, should be ok... 44MB/s!
	flash_controller_inst : entity work.flash_controller
	generic map
	(
		addr_bits =>flash_addr_bits
	)
	port map
	(
		CLK => CLK116,
		CLK_SLOW => CLK,
		RESET_N => RESET_N,

		-- Request from device 1 (cpu)
		flash_req1_addr_config => CPU_FLASH_CFG_REG,
		flash_req1_addr => CPU_FLASH_ADDR_REG(flash_addr_bits+1 downto 2),
		flash_req1_data_in => CPU_FLASH_DATA_REG,
		flash_req1_write_n => CPU_FLASH_WRITE_N_REG,

		flash_req2_addr(12 downto 1) => (others=>'0'),   -- first 2 32-bit words are config!
		flash_req2_addr(0 downto 0) => CONFIG_FLASH_ADDR(0 downto 0),

		flash_req3_addr(12 downto 8) => (others=>'0'),
		flash_req3_addr(7 downto 0) => "1"&ADPCM_STEP_ADDR(6 downto 0),

		flash_req4_addr(flash_addr_bits-1 downto 17) => (others=>'0'),
		flash_req4_addr(min(flash_addr_bits-1,16) downto 0) => SID_FLASH1_ADDR(min(flash_addr_bits-1,16) downto 0), --8KB per type: 6581, 8580 takes 16KB. Can use space after core for more?

		flash_req5_addr(flash_addr_bits-1 downto 17) => (others=>'0'),
		flash_req5_addr(min(flash_addr_bits-1,16) downto 0) => SID_FLASH2_ADDR(min(flash_addr_bits-1,16) downto 0), 

		flash_req6_addr(12 downto 9) => (others=>'0'),
		flash_req6_addr(8 downto 0) => "10"&PSG_PROFILESEL_REG&PSG_PROFILE_ADDR,  --TODO + init.bin

		flash_req7_addr(12 downto 9) => (others=>'0'),
		flash_req7_addr(8 downto 0) => "11"&SATURATE_REG&POKEY_PROFILE_ADDR,  --TODO + init.bin

		flash_req_request(0) => CPU_FLASH_REQUEST_REG,
		flash_req_request(1) => CONFIG_FLASH_REQUEST,
		flash_req_request(2) => ADPCM_STEP_REQUEST,
		flash_req_request(3) => SID_FLASH1_ROMREQUEST,
		flash_req_request(4) => SID_FLASH2_ROMREQUEST,
		flash_req_request(5) => PSG_PROFILE_REQUEST,
		flash_req_request(6) => POKEY_PROFILE_REQUEST,
		flash_req_request(7 downto 7) => (others=>'0'),
		flash_req_complete(7 downto 0) => open,

		flash_req_complete_slow(0) => CPU_FLASH_COMPLETE,
		flash_req_complete_slow(1) => CONFIG_FLASH_COMPLETE,
		flash_req_complete_slow(2) => ADPCM_STEP_READY,
		flash_req_complete_slow(3) => SID_FLASH1_ROMREADY,
		flash_req_complete_slow(4) => SID_FLASH2_ROMREADY,
		flash_req_complete_slow(5) => PSG_PROFILE_READY,
		flash_req_complete_slow(6) => POKEY_PROFILE_READY,
		flash_req_complete_slow(7 downto 7) => open,

		flash_data_out_slow => flash_do_slow
	);

	-- initialize the registers from flash

	-- flash memory map
	-- 32KB
	-- 0x0000-0x07ff - 2k configuration (regs, psg vol curve, pokey mixing curve, sid filter piecewise linear?)
	-- 	first 64 bits - config regs 
	-- 0x0800-0x3fff - 14k sid tables
	-- 0x4000-0x7fff - 16k fixed samples?

	-- reg init
	process(CONFIG_FLASH_STATE_REG, CONFIG_FLASH_COMPLETE)
	begin
		CONFIG_FLASH_REQUEST <= '0';
		CONFIG_FLASH_STATE_NEXT <= CONFIG_FLASH_STATE_REG;
		CONFIG_FLASH_ADDR <= "0";

		case CONFIG_FLASH_STATE_REG is
			when CONFIG_FLASH_STATE_ADDR1 =>
				CONFIG_FLASH_REQUEST <= '1';
				CONFIG_FLASH_ADDR <= "0";
				if (CONFIG_FLASH_COMPLETE='1') then
					CONFIG_FLASH_STATE_NEXT <= CONFIG_FLASH_STATE_ADDR2;
				end if;
			when CONFIG_FLASH_STATE_ADDR2 =>
				CONFIG_FLASH_REQUEST <= '1';
				CONFIG_FLASH_ADDR <= "1";
				if (CONFIG_FLASH_COMPLETE='1') then
					CONFIG_FLASH_STATE_NEXT <= CONFIG_FLASH_STATE_DONE;
				end if;
			when others=>
		end case;
	end process;
end generate;

	EXT_INT(0) <= '0';  --force to 0
	EXT_INT(17 downto ext_bits+1) <= (others=>'1');
	EXT_INT(18) <= CS0_N;
	EXT_INT(19) <= CS1;
	EXT_INT(20) <= '1';
	EXT_INT(ext_bits downto 1) <= EXT;

        synchronizer_gtia_audio : entity work.synchronizer
                port map (clk=>clk, raw=>EXT_INT(gtia_audio_bit), sync=>GTIA_AUDIO);
        synchronizer_fancy_enable : entity work.synchronizer
		port map (clk=>clk, raw=>EXT_INT(fancy_switch_bit), sync=>FANCY_SWITCH);

	--assert address_bits<7 report "EXT3 already used for A6";

	CLK_OUT <= OSC_CLK;


pll_v2_inst : if pll_v2=1 generate
	pll_inst : pll
	PORT MAP(inclk0 => CLK_SLOW,
			 c0 => CLK, --56 ish
			 c1 => CLK116,  --113ish
			 c2 => CLK106,  --106ish
			 locked => RESET_N);
end generate;

pll_v3_inst : if pll_v2=0 generate
	pll_inst : pllv3
	PORT MAP(inclk0 => CLK0, --49.192 (50 on prototype)
			 c0 => CLK, --49.192 
			 c1 => CLK116,  --113ish
			 c2 => CLK106,  --106ish
			 c3 => CLK6144,  --6.44MHz
			 locked => RESET_N);
end generate;


	AIN(3 downto 0) <= A;
	AIN(7) <= EXT_INT(a7_bit);
	AIN(6) <= EXT_INT(a6_bit);
	AIN(5) <= EXT_INT(a5_bit);
	AIN(4) <= EXT_INT(a4_bit);
	CS1MOD <= EXT_INT(cs1_bit);
	CS0NMOD <= EXT_INT(cs0_bit);

bus_adapt : entity work.slave_timing_6502
	GENERIC MAP
	(
		address_bits => 8
	)
	PORT MAP
	(
		CLK => CLK,
		RESET_N => RESET_N,
		
		-- input from the cart port
		PHI2 => PHI2,
		bus_addr => AIN, 
		bus_data => D,
	
		-- output to the cart port
		bus_data_out => BUS_DATA,
		bus_drive => BUS_OE,
		bus_cs => CS_COMB,
		bus_rw_n => W_N,

		-- request for a memory bus cycle (read or write)
		BUS_REQUEST => REQUEST,
		ADDR_IN => ADDR_IN,
		DATA_IN => WRITE_DATA,
		RW_N => WRITE_N,

		-- end of cycle
		ENABLE_CYCLE => ENABLE_CYCLE,
		ENABLE_DOUBLE_CYCLE => ENABLE_DOUBLE_CYCLE,

		DATA_OUT => DO_MUX,
		DRIVE_DATA_OUT => DRIVE_DO_MUX
	);
	
auto_stereo : if enable_auto_stereo=1 generate -- auto detect
	a4 : ENTITY work.stereo_detect
	PORT MAP
	( 
		CLK => clk,
		RESET_N => reset_n,
	
		A => AIN(4), -- raw...
		DETECT => A4_DETECTED
	);
end generate;

auto_stereo_off : if enable_auto_stereo=0 generate -- manual switch
	A4_DETECTED <= '1';
end generate;

FANCY_ENABLE <= FANCY_SWITCH and A4_DETECTED;
	
-- TODO: into another entity
process(clk)
begin
	if (clk'event and clk='1') then
		CHANNEL0SUM_REG <= CHANNEL0SUM_NEXT;
		CHANNEL1SUM_REG <= CHANNEL1SUM_NEXT;
		CHANNEL2SUM_REG <= CHANNEL2SUM_NEXT;
		CHANNEL3SUM_REG <= CHANNEL3SUM_NEXT;
	end if;
end process;

	
process(
	POKEY_CHANNEL0,POKEY_CHANNEL1,POKEY_CHANNEL2,POKEY_CHANNEL3,
	CHANNEL_MODE_REG -- 0=pokeys have a channel each,1=ch 0 summed, ch 1 summed, ch 2 summed etc
	)
variable p0 : unsigned(5 downto 0);	
variable p1 : unsigned(5 downto 0);
variable p2 : unsigned(5 downto 0);
variable p3 : unsigned(5 downto 0);

variable c0 : unsigned(5 downto 0);	
variable c1 : unsigned(5 downto 0);
variable c2 : unsigned(5 downto 0);
variable c3 : unsigned(5 downto 0);	
	
variable sum0 : unsigned(5 downto 0);	
variable sum1 : unsigned(5 downto 0);
variable sum2 : unsigned(5 downto 0);
variable sum3 : unsigned(5 downto 0);

begin
	p0 := resize(unsigned(POKEY_CHANNEL0(0)),6) + resize(unsigned(POKEY_CHANNEL1(0)),6) + resize(unsigned(POKEY_CHANNEL2(0)),6) + resize(unsigned(POKEY_CHANNEL3(0)),6);
	p1 := resize(unsigned(POKEY_CHANNEL0(1)),6) + resize(unsigned(POKEY_CHANNEL1(1)),6) + resize(unsigned(POKEY_CHANNEL2(1)),6) + resize(unsigned(POKEY_CHANNEL3(1)),6);
	p2 := resize(unsigned(POKEY_CHANNEL0(2)),6) + resize(unsigned(POKEY_CHANNEL1(2)),6) + resize(unsigned(POKEY_CHANNEL2(2)),6) + resize(unsigned(POKEY_CHANNEL3(2)),6);
	p3 := resize(unsigned(POKEY_CHANNEL0(3)),6) + resize(unsigned(POKEY_CHANNEL1(3)),6) + resize(unsigned(POKEY_CHANNEL2(3)),6) + resize(unsigned(POKEY_CHANNEL3(3)),6);
	
	c0 := resize(unsigned(POKEY_CHANNEL0(0)),6) + resize(unsigned(POKEY_CHANNEL0(1)),6) + resize(unsigned(POKEY_CHANNEL0(2)),6) + resize(unsigned(POKEY_CHANNEL0(3)),6);
	c1 := resize(unsigned(POKEY_CHANNEL1(0)),6) + resize(unsigned(POKEY_CHANNEL1(1)),6) + resize(unsigned(POKEY_CHANNEL1(2)),6) + resize(unsigned(POKEY_CHANNEL1(3)),6);
	c2 := resize(unsigned(POKEY_CHANNEL2(0)),6) + resize(unsigned(POKEY_CHANNEL2(1)),6) + resize(unsigned(POKEY_CHANNEL2(2)),6) + resize(unsigned(POKEY_CHANNEL2(3)),6);
	c3 := resize(unsigned(POKEY_CHANNEL3(0)),6) + resize(unsigned(POKEY_CHANNEL3(1)),6) + resize(unsigned(POKEY_CHANNEL3(2)),6) + resize(unsigned(POKEY_CHANNEL3(3)),6);	
	
	if CHANNEL_MODE_REG ='1' then
		sum0 := c0;
		sum1 := c1;
		sum2 := c2;
		sum3 := c3;	
	else
		sum0 := p0;
		sum1 := p1;
		sum2 := p2;
		sum3 := p3;	
	end if;
	
	CHANNEL0SUM_NEXT <= sum0;
	CHANNEL1SUM_NEXT <= sum1;
	CHANNEL2SUM_NEXT <= sum2;
	CHANNEL3SUM_NEXT <= sum3;
end process;
	
pokey_mixer_both : entity work.pokey_mixer_mux
PORT MAP(CLK => CLK,
		RESET_N => RESET_N,
		 CHANNEL_0 => CHANNEL0SUM_REG,
		 CHANNEL_1 => CHANNEL1SUM_REG,
		 CHANNEL_2 => CHANNEL2SUM_REG,
		 CHANNEL_3 => CHANNEL3SUM_REG,
		 VOLUME_OUT_0 => POKEY_AUDIO_0,
		 VOLUME_OUT_1 => POKEY_AUDIO_1,
		 VOLUME_OUT_2 => POKEY_AUDIO_2,
		 VOLUME_OUT_3 => POKEY_AUDIO_3,
		 PROFILE_ADDR => POKEY_PROFILE_ADDR,
		 PROFILE_REQUEST => POKEY_PROFILE_REQUEST,
		 PROFILE_READY => POKEY_PROFILE_READY,
		 PROFILE_DATA => flash_do_slow(15 downto 0)
		 );

flash_off : if enable_flash=0 generate 
	shared_pokey_mixer : entity work.pokey_mixer
	port map
	(
		sum => unsigned(pokey_profile_addr),

		saturate => saturate_reg,

		VOLUME_OUT_NEXT => flash_do_slow(15 downto 0)
	);
	POKEY_PROFILE_READY <= '1';
end generate;

--------------------------------------------------------
-- PRIMARY POKEY		 GTIA_VOLUME_
--------------------------------------------------------
pokey1 : entity work.pokey
GENERIC MAP
(
	custom_keyboard_scan => 1
)
PORT MAP(CLK => CLK,
		 ENABLE_179 => ENABLE_CYCLE,
		 WR_EN => POKEY_WRITE_ENABLE(0),
		 RESET_N => RESET_N,
		 SIO_IN1 => SIO_RXD,
		 SIO_CLOCKIN_IN => SIO_CLOCKIN_IN,
		 SIO_CLOCKIN_OUT => SIO_CLOCKIN_OUT,
		 SIO_CLOCKIN_OE => SIO_CLOCKIN_OE,
		 ADDR => ADDR_IN(3 DOWNTO 0),
		 DATA_IN => WRITE_DATA(7 DOWNTO 0),
		 keyboard_response => KEYBOARD_RESPONSE,
		 POT_IN => PADDLE,
		 IRQ_N_OUT => POKEY_IRQ(0),
		 SIO_OUT1 => SIO_TXD,
		 SIO_OUT2 => open,
		 SIO_OUT3 => open,
		 SIO_CLOCKOUT => SIO_CLOCKOUT,
		 POT_RESET => POTRESET,
		 CHANNEL_0_OUT => POKEY_CHANNEL0(0),
		 CHANNEL_1_OUT => POKEY_CHANNEL1(0),
		 CHANNEL_2_OUT => POKEY_CHANNEL2(0),
		 CHANNEL_3_OUT => POKEY_CHANNEL3(0),
		 DATA_OUT => POKEY_DO(0),
		 keyboard_scan => KEYBOARD_SCAN,
		 keyboard_scan_enable => KEYBOARD_SCAN_ENABLE,
		 keyboard_scan_update => KEYBOARD_SCAN_UPDATE
		);

--------------------------------------------------------		
-- POKEY 2-4	 
--------------------------------------------------------		
   POKEY_OFF: 
   for I in pokeys to 3 generate
      POKEY_CHANNEL0(I) <= (others=>'0');
		POKEY_CHANNEL1(I) <= (others=>'0');
		POKEY_CHANNEL2(I) <= (others=>'0');
		POKEY_CHANNEL3(I) <= (others=>'0');
		POKEY_IRQ(I) <= '1';
		POKEY_DO(I) <= (others=>'0');
   end generate POKEY_OFF;		

   POKEY_ON: 
   for I in 1 to pokeys-1 generate
		pokeyx : entity work.pokey
		GENERIC MAP
		(
			custom_keyboard_scan => 2
		)
		PORT MAP(CLK => CLK,
				 ENABLE_179 => ENABLE_CYCLE,
				 WR_EN => POKEY_WRITE_ENABLE(I),
				 RESET_N => RESET_N,
				 ADDR => ADDR_IN(3 DOWNTO 0),
				 DATA_IN => WRITE_DATA(7 DOWNTO 0),
				 CHANNEL_0_OUT => POKEY_CHANNEL0(I),
				 CHANNEL_1_OUT => POKEY_CHANNEL1(I),
				 CHANNEL_2_OUT => POKEY_CHANNEL2(I),
				 CHANNEL_3_OUT => POKEY_CHANNEL3(I),
				 DATA_OUT => POKEY_DO(I),
				 SIO_IN1 => '1',
				 IRQ_N_OUT => POKEY_IRQ(I),
				 keyboard_response => "00",
				 pot_in=>"00000000");
   end generate POKEY_ON;

--------------
-- SID or PSG!
--------------
sidpsg_on : if enable_sid=1 or enable_psg=1 generate
	clockgen1 : entity work.clockgen
	PORT MAP
	(
		CLK => CLK,
		RESET_N => (RESET_N and (PAL_REG xnor PAL_NEXT)),

		PAL => pal_reg,
		PHI2 => ENABLE_CYCLE,

		MHZ1 => MHZ1_ENABLE,
		MHZ2 => MHZ2_ENABLE
	);
end generate sidpsg_on;

--------------------------------------------------------
-- SID
--------------------------------------------------------
sid_off : if enable_sid=0 generate 
	SID_AUDIO(0) <= (others=>'0');
	SID_AUDIO(1) <= (others=>'0');
	SID_DO(0) <= (others=>'0');
	SID_DO(1) <= (others=>'0');
	SID_DRIVE_DO(0) <= '0';
	SID_DRIVE_DO(1) <= '0';
end generate sid_off;

sid_on : if enable_sid=1 generate 

sid_clk_on : if ext_clk_enable=0 generate 
	SID_CLK_ENABLE <= MHZ1_ENABLE;
end generate sid_clk_on;

sid_clk_off : if ext_clk_enable=1 generate 
        synchronizer_sid_clk : entity work.synchronizer
		port map (clk=>clk, raw=>PADDLE(6), sync=>SID_ENABLE_NEXT);
	SID_CLK_ENABLE <= SID_ENABLE_NEXT and not(SID_ENABLE_REG);

process(clk,reset_n)
begin
	if (reset_n='0') then
		SID_ENABLE_REG <= '0';
	elsif (clk'event and clk='1') then
		SID_ENABLE_REG <= SID_ENABLE_NEXT;
	end if;
end process;
end generate sid_clk_off;

f_distortion_mux : entity work.SID_f_distortion_mux
port map
(
	clk=>clk,
	reset_n=>reset_n,
	state1=>SID1_FILTER_BP(17 downto 8),
	state2=>SID1_FILTER_HP(17 downto 8),
	state3=>SID2_FILTER_BP(17 downto 8),
	state4=>SID2_FILTER_HP(17 downto 8),
	SIDTYPE12 => SID_FILTER1_REG(0),
	SIDTYPE34 => SID_FILTER2_REG(0),
	f_raw12=>unsigned(SID1_F_RAW),
	f_raw34=>unsigned(SID2_F_RAW),
	f_distorted1=>SID1_F_BP,
	f_distorted2=>SID1_F_HP,
	f_distorted3=>SID2_F_BP,
	f_distorted4=>SID2_F_HP
);

sid1 : entity work.SID_top
GENERIC MAP
(
	wave_base => std_logic_vector(to_unsigned(sid_wave_base,17))
)
PORT MAP(
	CLK => CLK,
	RESET_N => RESET_N,
	ENABLE => SID_CLK_ENABLE, --1MHz

	WRITE_ENABLE => SID_WRITE_ENABLE(0),
	READ_ENABLE => SID_READ_ENABLE(0),
	ADDR => ADDR_IN(4 downto 0),
	DI => WRITE_DATA(7 downto 0),
	DO => SID_DO(0),
	DRIVE_DO => SID_DRIVE_DO(0),
	--POT_X => (others=>'0'),
	--POT_Y => (others=>'0'),
	--EXTFILTER_EN => '0',
	AUDIO => SID_AUDIO(0), 

	SIDTYPE => SID_FILTER1_REG(0),
	EXT => "0"&SID_FILTER1_REG(1),
	EXT_ADC => (others=>'0'),

	POT_X => '0',
	POT_Y => '0',

	rom_addr => sid_flash1_addr,
	rom_data => flash_do_slow,
       	rom_request => sid_flash1_romrequest,
	rom_ready => sid_flash1_romready,

	FILTER_BP_OUT => SID1_FILTER_BP,
	FILTER_HP_OUT => SID1_FILTER_HP,
	FILTER_F_OUT => SID1_F_RAW,
	FILTER_F_BP => std_logic_vector(SID1_F_BP),
	FILTER_F_HP => std_logic_vector(SID1_F_HP)
);

sid2 : entity work.SID_top
GENERIC MAP
(
	wave_base => std_logic_vector(to_unsigned(sid_wave_base,17))
)
PORT MAP(
	CLK => CLK,
	RESET_N => RESET_N,
	ENABLE => SID_CLK_ENABLE, --1MHz

	WRITE_ENABLE => SID_WRITE_ENABLE(1),
	READ_ENABLE => SID_READ_ENABLE(1),
	ADDR => ADDR_IN(4 downto 0),
	DI => WRITE_DATA(7 downto 0),
	DO => SID_DO(1),
	DRIVE_DO => SID_DRIVE_DO(1),
	--POT_X => (others=>'0'),
	--POT_Y => (others=>'0'),
	--EXTFILTER_EN => '0',
	AUDIO => SID_AUDIO(1),

	SIDTYPE => SID_FILTER2_REG(0),
	EXT => "0"&SID_FILTER2_REG(1),
	EXT_ADC => (others=>'0'),

	POT_X => '0',
	POT_Y => '0',

	rom_addr => sid_flash2_addr,
	rom_data => flash_do_slow,
       	rom_request => sid_flash2_romrequest,
	rom_ready => sid_flash2_romready,

	FILTER_BP_OUT => SID2_FILTER_BP,
	FILTER_HP_OUT => SID2_FILTER_HP,
	FILTER_F_OUT => SID2_F_RAW,
	FILTER_F_BP => std_logic_vector(SID2_F_BP),
	FILTER_F_HP => std_logic_vector(SID2_F_HP)
);
end generate sid_on;		
--------------------------------------------------------
-- PSG
--------------------------------------------------------
psg_off : if enable_psg=0 generate 
	PSG_AUDIO(0) <= (others=>'0');
	PSG_AUDIO(1) <= (others=>'0');
	PSG_DO(0) <= (others=>'0');
	PSG_DO(1) <= (others=>'0');
end generate psg_off;

-- VERY approx (for now) PSG master clock!
psg_on : if enable_psg=1 generate 
psg_clk_on : if ext_clk_enable=0 generate 
	PSG_ENABLE_2MHz <= MHZ2_ENABLE;
	PSG_ENABLE_1MHz <= MHZ1_ENABLE;

process(PSG_FREQ_REG,PSG_ENABLE_2MHz,PSG_ENABLE_1MHz,ENABLE_CYCLE)
begin
	PSG_ENABLE <= '0';

	case PSG_FREQ_REG is
		when "00"=>
			PSG_ENABLE <= PSG_ENABLE_2MHz;
		when "01"=>
			PSG_ENABLE <= PSG_ENABLE_1MHz;
		when others=>
			PSG_ENABLE <= ENABLE_CYCLE;
	end case;
end process;

end generate psg_clk_on;

psg_clk_off : if ext_clk_enable=1 generate 
        synchronizer_sid_clk : entity work.synchronizer
                port map (clk=>clk, raw=>PADDLE(7), sync=>PSG_ENABLE_NEXT);

	PSG_ENABLE <= PSG_ENABLE_NEXT and not(PSG_ENABLE_REG);
process(clk,reset_n)
begin
	if (reset_n='0') then
		PSG_ENABLE_REG <= '0'; 
	elsif (clk'event and clk='1') then
		PSG_ENABLE_REG <= PSG_ENABLE_NEXT;
	end if;
end process;
end generate psg_clk_off;

process(PSG_STEREOMODE_REG)
begin
	PSG_MIX1 <= (others=>'0');
	PSG_MIX2 <= (others=>'0');

	case PSG_STEREOMODE_REG is
		when "00"=>
			PSG_MIX1 <= "111111";
			PSG_MIX2 <= "111111";
		when "01"=>
			PSG_MIX1 <= "110110";
			PSG_MIX2 <= "011011";
		when "10"=>
			PSG_MIX1 <= "101101";
			PSG_MIX2 <= "011011";
		when others=>
			PSG_MIX1 <= "111000";
			PSG_MIX2 <= "000111";
	end case;
end process;

PSG_1 : entity work.PSG_top
  port map(
	clk=>clk,
	reset_n=>reset_n,
	enable=>psg_enable,
	addr=>addr_in(3 downto 0),
	write_enable=>PSG_WRITE_ENABLE(0),
	ENVELOPE32=>not(PSG_ENVELOPE16_REG),
	di=>write_data,
	do=>PSG_DO(0),
	channel_a_vol => PSG_CHANNEL(0),
	channel_b_vol => PSG_CHANNEL(1),
	channel_c_vol => PSG_CHANNEL(2),
	channel_changed => PSG_CHANGED(0)
	);
	
PSG_2 : entity work.PSG_top
  port map(
	clk=>clk,
	reset_n=>reset_n,
	enable=>psg_enable, 
	addr=>addr_in(3 downto 0),
	write_enable=>PSG_WRITE_ENABLE(1),
	ENVELOPE32=>not(PSG_ENVELOPE16_REG),
	di=>write_data,
	do=>PSG_DO(1),
	channel_a_vol => PSG_CHANNEL(3),
	channel_b_vol => PSG_CHANNEL(4),
	channel_c_vol => PSG_CHANNEL(5),
	channel_changed => PSG_CHANGED(1)
	);

	vol_profile1 : entity work.PSG_volume_profile
	PORT MAP
	( 
		CLK => clk,
		RESET_N => reset_n,		
		
		CHANNEL_1A => PSG_CHANNEL(0),
		CHANNEL_1B => PSG_CHANNEL(1),
		CHANNEL_1C => PSG_CHANNEL(2),
		CHANNEL_1_CHANGED => PSG_CHANGED(0),
		CHANNEL_2A => PSG_CHANNEL(3),
		CHANNEL_2B => PSG_CHANNEL(4),
		CHANNEL_2C => PSG_CHANNEL(5),
		CHANNEL_2_CHANGED => PSG_CHANGED(1),

		CHANNEL_MASK_1=>PSG_MIX1, --LABC:RABC
		CHANNEL_MASK_2=>PSG_MIX2,

		AUDIO_OUT_1 => PSG_AUDIO(0),
		AUDIO_OUT_2 => PSG_AUDIO(1),

		--PROFILE_SELECT=>PSG_PROFILESEL_REG,
		PROFILE_ADDR => PSG_PROFILE_ADDR,
		PROFILE_REQUEST => PSG_PROFILE_REQUEST,
		PROFILE_READY => PSG_PROFILE_READY,
		PROFILE_DATA => flash_do_slow(15 downto 0)
	);	

end generate psg_on;		
	
--------------------------------------------------------
-- COVOX
--------------------------------------------------------
covox_off : if enable_covox=0 generate 
	SAMPLE_IRQ <= '0';
	SAMPLE_DO <= (others=>'0');
	SAMPLE_AUDIO(0) <= (others=>'0');
	SAMPLE_AUDIO(1) <= (others=>'0');
	ADPCM_STEP_REQUEST <= '0';
end generate covox_off;

covox_on : if enable_covox=1 and enable_sample=0 generate 
	SAMPLE_IRQ <= '0';
	ADPCM_STEP_REQUEST <= '0';

	covox1 : entity work.covox_top
	PORT MAP(
		CLK => CLK,
		RESET_N => RESET_N,
	
		WRITE_ENABLE => SAMPLE_WRITE_ENABLE,
		ADDR => ADDR_IN(1 downto 0),
		DI => WRITE_DATA(7 downto 0),
		DO => SAMPLE_DO,
		AUDIO0 => SAMPLE_AUDIO(0),
		AUDIO1 => SAMPLE_AUDIO(1) 
	);
end generate covox_on;

----------------------------------------

sample_on : if enable_sample=1 generate 

	sample1 : entity work.sample_top
	PORT MAP(
		CLK => CLK,
		RESET_N => RESET_N,

		ENABLE => ENABLE_DOUBLE_CYCLE, 
		REQUEST => REQUEST,
	
		WRITE_ENABLE => SAMPLE_WRITE_ENABLE,
		ADDR => ADDR_IN(4 downto 0),
		DI => WRITE_DATA(7 downto 0),
		DO => SAMPLE_DO,
		AUDIO0 => SAMPLE_AUDIO(0),
		AUDIO1 => SAMPLE_AUDIO(1),
		IRQ => SAMPLE_IRQ,
		
		RAM_ADDR => SAMPLE_RAM_ADDRESS,
		RAM_WRITE_ENABLE => SAMPLE_RAM_WRITE_ENABLE,
		RAM_DATA => SAMPLE_RAM_DATA,

		ADPCM_STEP_ADDR => ADPCM_STEP_ADDR,
		ADPCM_STEP_REQUEST => ADPCM_STEP_REQUEST,
		ADPCM_STEP_READY => ADPCM_STEP_READY,
		ADPCM_STEP_VALUE => FLASH_DO_SLOW(14 downto 0)
	);

	sample_ram_inst : entity work.generic_ram_infer
	GENERIC MAP
	(
		ADDRESS_WIDTH => 16,
		SPACE => 43008,
		DATA_WIDTH => 8
	)
	PORT MAP
	(
	        clock => clk,
		reset_n => reset_n,
		data => write_data,
		address => sample_ram_address,
		we => sample_ram_write_enable,
		q => sample_ram_data
	);

end generate sample_on;
		
-------------------------------------------------------
-- COMMON, data bus
--
--
-- memory map
-- d200 - pokey0
-- d210 - pokey1
-- d220 - pokey2
-- d230 - pokey3
-- d240 - sid1
-- d260 - sid2
-- d280 - covox/sample
-- d2a0 - ym1 (mapped as 0-f, rather than convoluted 0/1)
-- d2b0 - ym2
-- d2f0 - config (write 0x3f to d21c to map it in d210, for low bit devices)

process(CONFIG_ENABLE_REG,ADDR_IN,addr_decoded4,FANCY_ENABLE)
	variable addr_bits : std_logic_vector(3 downto 0);
begin
	-- choose which bank
	addr_bits := (others=>'0');
	addr_bits(3 downto 0) := ADDR_IN(7 downto 4);
	
	if (fancy_enable='0') then
		addr_bits := (others=>'0');
	end if;

	if (addr_in(7 downto 4)=x"f") then -- TODO: tweak...
		addr_bits := x"0"; --disable config access here
	end if;

	if ((config_enable_reg='1' and addr_bits="0001") or (addr_bits(3 downto 2) = "00" and addr_decoded4(12)='1')) then
		addr_bits := x"f";
	end if;
	
	DEVICE_ADDR <= addr_bits;
end process;			

process(
	DEVICE_ADDR,
	POKEY_DO,
	SID_DO,SID_DRIVE_DO,
	PSG_DO,
	SAMPLE_DO,
	CONFIG_DO,
	write_n,
	request,
	RESTRICT_CAPABILITY_REG, readreq_s, writereq_s
	)
	variable writereq : std_logic;
	variable readreq : std_logic;
	variable enable_region : std_logic;
begin
	writereq := not(write_n) and request;
	readreq := write_n and request;
	
	POKEY_WRITE_ENABLE <= (others=>'0');
	SID_WRITE_ENABLE <= (others=>'0');
	SID_READ_ENABLE <= (others=>'0');
	PSG_WRITE_ENABLE <= (others=>'0');
	SAMPLE_WRITE_ENABLE <= '0';
	CONFIG_WRITE_ENABLE <= '0';
	enable_region :='0';
	
	DO_MUX <= (others =>'0');
	DRIVE_DO_MUX <= '1';
	
	case DEVICE_ADDR is
		when "0001" =>
			enable_region := RESTRICT_CAPABILITY_REG(0) or RESTRICT_CAPABILITY_REG(1);
			DO_MUX <= POKEY_DO(1);
			POKEY_WRITE_ENABLE(1) <= writereq_s;
		when "0010" =>
			enable_region := RESTRICT_CAPABILITY_REG(1);
			DO_MUX <= POKEY_DO(2);
			POKEY_WRITE_ENABLE(2) <= writereq_s;
		when "0011" =>
			enable_region := RESTRICT_CAPABILITY_REG(1);
			DO_MUX <= POKEY_DO(3);
			POKEY_WRITE_ENABLE(3) <= writereq_s;
		when "0100"|"0101" =>
			enable_region := RESTRICT_CAPABILITY_REG(2);
			DO_MUX <= SID_DO(0);
			DRIVE_DO_MUX <= SID_DRIVE_DO(0);
			SID_WRITE_ENABLE(0) <= writereq_s;
			SID_READ_ENABLE(0) <= readreq_s;
		when "0110"|"0111" =>
			enable_region := RESTRICT_CAPABILITY_REG(2);
			DO_MUX <= SID_DO(1);
			DRIVE_DO_MUX <= SID_DRIVE_DO(1);
			SID_WRITE_ENABLE(1) <= writereq_s;
			SID_READ_ENABLE(0) <= readreq_s;
		when "1000"|"1001" =>
			enable_region := RESTRICT_CAPABILITY_REG(4);
			DO_MUX <= SAMPLE_DO;								
			SAMPLE_WRITE_ENABLE <= writereq_s;			
		when "1010" =>
			enable_region := RESTRICT_CAPABILITY_REG(3);
			DO_MUX <= PSG_DO(0);
			PSG_WRITE_ENABLE(0) <= writereq_s;
		when "1011" =>
			enable_region := RESTRICT_CAPABILITY_REG(3);
			DO_MUX <= PSG_DO(1);			
			PSG_WRITE_ENABLE(1) <= writereq_s;
		when "1111" =>
			enable_region := '1';
			DO_MUX <= CONFIG_DO;
			CONFIG_WRITE_ENABLE <= writereq_s;
		when others =>
	end case;

	readreq_s <= readreq and enable_region;
	writereq_s <= writereq and enable_region;

	if enable_region='0' then
		DO_MUX <= POKEY_DO(0);
		POKEY_WRITE_ENABLE(0) <= writereq;
	end if;
end process;

-------------------------------------------------------
-- Configuration

process(clk,reset_n)
begin
	if (reset_n='0') then
		if detect_right_on_by_default=1 then
			DETECT_RIGHT_REG <= '1';
		else
			DETECT_RIGHT_REG <= '0';
		end if;
		IRQ_EN_REG <= '0';
		CHANNEL_MODE_REG <= '0';
		if saturate_on_by_default=1 then
			SATURATE_REG <= '1';
		else
			SATURATE_REG <= '0';
		end if;
		POST_DIVIDE_REG <= "10100000"; -- 1/2 5v, 3/4 1v
		GTIA_ENABLE_REG <= "1100"; -- external only
		CONFIG_ENABLE_REG <= '0';
		VERSION_LOC_REG <= (others=>'0');
		PAL_REG <= '1';
		PSG_FREQ_REG <= "00"; --2MHz
		PSG_STEREOMODE_REG <= "01"; --Polish
		PSG_PROFILESEL_REG <= "00"; --Simple log
		PSG_ENVELOPE16_REG <= '0'; --32 step
		SID_FILTER1_REG <= "10"; -- 0=8580,1=6581,2=digifix
		SID_FILTER2_REG <= "10"; -- 0=8580,1=6581,2=digifix
		RESTRICT_CAPABILITY_REG <= (others=>'1');
		CHANNEL_EN_REG <= (others=>'1');
	elsif (clk'event and clk='1') then
		DETECT_RIGHT_REG <= DETECT_RIGHT_NEXT;
		IRQ_EN_REG <= IRQ_EN_NEXT;
		CHANNEL_MODE_REG <= CHANNEL_MODE_NEXT;
		SATURATE_REG <= SATURATE_NEXT;
		POST_DIVIDE_REG <= POST_DIVIDE_NEXT;
		GTIA_ENABLE_REG <= GTIA_ENABLE_NEXT;
		CONFIG_ENABLE_REG <= CONFIG_ENABLE_NEXT;
		VERSION_LOC_REG <= VERSION_LOC_NEXT;
		PAL_REG <= PAL_NEXT;
		PSG_FREQ_REG <= PSG_FREQ_NEXT;
		PSG_STEREOMODE_REG <= PSG_STEREOMODE_NEXT;
		PSG_PROFILESEL_REG <= PSG_PROFILESEL_NEXT;
		PSG_ENVELOPE16_REG <= PSG_ENVELOPE16_NEXT;
		SID_FILTER1_REG <= SID_FILTER1_NEXT;
		SID_FILTER2_REG <= SID_FILTER2_NEXT;
		RESTRICT_CAPABILITY_REG <= RESTRICT_CAPABILITY_NEXT;
		CHANNEL_EN_REG <= CHANNEL_EN_NEXT;
	end if;
end process;

-- default config

gen_config : if enable_config=1 generate

decode_addr1 : entity work.complete_address_decoder
	generic map(width=>4)
	port map (addr_in=>ADDR_IN(3 downto 0), addr_decoded=>addr_decoded4);

process(CONFIG_WRITE_ENABLE, WRITE_DATA, addr_decoded4,
	SATURATE_REG,CHANNEL_MODE_REG,IRQ_EN_REG,DETECT_RIGHT_REG,
	CONFIG_ENABLE_REG,
	POST_DIVIDE_REG,
	GTIA_ENABLE_REG,
	VERSION_LOC_REG,
	PSG_FREQ_REG,
	PSG_STEREOMODE_REG,
	PSG_PROFILESEL_REG,
	PSG_ENVELOPE16_REG,
	SID_FILTER1_REG, SID_FILTER2_REG,
	CPU_FLASH_REQUEST_REG,CPU_FLASH_WRITE_N_REG,CPU_FLASH_CFG_REG,CPU_FLASH_ADDR_REG,CPU_FLASH_DATA_REG,
	CPU_FLASH_COMPLETE,CONFIG_FLASH_COMPLETE,CONFIG_FLASH_ADDR,flash_do_slow,
	RESTRICT_CAPABILITY_REG,
	CHANNEL_EN_REG,
	PAL_REG
)
begin
	SATURATE_NEXT <= SATURATE_REG;
	CHANNEL_MODE_NEXT <= CHANNEL_MODE_REG;
	IRQ_EN_NEXT <= IRQ_EN_REG;
	DETECT_RIGHT_NEXT <= DETECT_RIGHT_REG;

	POST_DIVIDE_NEXT <= POST_DIVIDE_REG;
	
	GTIA_ENABLE_NEXT <= GTIA_ENABLE_REG;
	
	CONFIG_ENABLE_NEXT <= CONFIG_ENABLE_REG;
	
	VERSION_LOC_NEXT <= VERSION_LOC_REG;

	PSG_FREQ_NEXT <= PSG_FREQ_REG;
	PSG_STEREOMODE_NEXT <= PSG_STEREOMODE_REG;
	PSG_PROFILESEL_NEXT <= PSG_PROFILESEL_REG;
	PSG_ENVELOPE16_NEXT <= PSG_ENVELOPE16_REG;

	SID_FILTER1_NEXT <= SID_FILTER1_REG;
	SID_FILTER2_NEXT <= SID_FILTER2_REG;

	CPU_FLASH_REQUEST_NEXT <= CPU_FLASH_REQUEST_REG;
	CPU_FLASH_WRITE_N_NEXT <= CPU_FLASH_WRITE_N_REG;
	CPU_FLASH_CFG_NEXT <= CPU_FLASH_CFG_REG;
	CPU_FLASH_ADDR_NEXT <= CPU_FLASH_ADDR_REG;
	CPU_FLASH_DATA_NEXT <= CPU_FLASH_DATA_REG;

	RESTRICT_CAPABILITY_NEXT <= RESTRICT_CAPABILITY_REG;
	CHANNEL_EN_NEXT <= CHANNEL_EN_REG;

	PAL_NEXT <= PAL_REG;

	if (CPU_FLASH_COMPLETE='1') then
		CPU_FLASH_DATA_NEXT <= flash_do_slow;
		CPU_FLASH_REQUEST_NEXT <= '0';
	end if;

	if (enable_flash=1 and CONFIG_FLASH_COMPLETE='1') then
		case CONFIG_FLASH_ADDR is
			when "0"=>
				SATURATE_NEXT <= flash_do_slow(0);
					-- 1 reserved...
				CHANNEL_MODE_NEXT <= flash_do_slow(2);
				IRQ_EN_NEXT <= flash_do_slow(3);
				DETECT_RIGHT_NEXT <= flash_do_slow(4);
				PAL_NEXT <= flash_do_slow(5);
					-- 6-7 reserved
				POST_DIVIDE_NEXT <= flash_do_slow(15 downto 8);
				GTIA_ENABLE_NEXT <= flash_do_slow(19 downto 16);
					-- 23 downto 20 reserved
				PSG_FREQ_NEXT <= flash_do_slow(25 downto 24);
				PSG_STEREOMODE_NEXT <= flash_do_slow(27 downto 26);
				PSG_ENVELOPE16_NEXT <= flash_do_slow(28);
				PSG_PROFILESEL_NEXT <= flash_do_slow(30 downto 29);
					-- 31 reserved
			when "1" =>
				SID_FILTER1_NEXT <= flash_do_slow(1 downto 0);
				-- 2-3 reserved
				SID_FILTER2_NEXT <= flash_do_slow(5 downto 4);
				-- 6-7 reserved
				RESTRICT_CAPABILITY_NEXT <= flash_do_slow(12 downto 8);
				-- 13-15 reserved
				-- 16-23 reserved (used in sidmax)
				CHANNEL_EN_NEXT <= flash_do_slow(28 downto 24);
				-- 29-31 reserved
			when others =>
		end case;
	elsif (CONFIG_WRITE_ENABLE='1') then
		if (addr_decoded4(0)='1') then
			SATURATE_NEXT <= WRITE_DATA(0);
			CHANNEL_MODE_NEXT <= WRITE_DATA(2);
			IRQ_EN_NEXT <= WRITE_DATA(3);
			DETECT_RIGHT_NEXT <= WRITE_DATA(4);
			PAL_NEXT <= WRITE_DATA(5);
		end if;

		if (addr_decoded4(2)='1') then
			POST_DIVIDE_NEXT <= WRITE_DATA;
		end if;
				
		if (addr_decoded4(3)='1') then			
			GTIA_ENABLE_NEXT <= WRITE_DATA(3 downto 0);
		end if;		

		if (addr_decoded4(4)='1') then
			VERSION_LOC_NEXT <= WRITE_DATA(2 downto 0);
		end if;
		
		if (addr_decoded4(5)='1') then
			PSG_FREQ_NEXT <= WRITE_DATA(1 downto 0);
			PSG_STEREOMODE_NEXT <= WRITE_DATA(3 downto 2);
			PSG_ENVELOPE16_NEXT <= WRITE_DATA(4);
			PSG_PROFILESEL_NEXT <= WRITE_DATA(6 downto 5);
		end if;

		if (addr_decoded4(6)='1') then
			SID_FILTER1_NEXT <= WRITE_DATA(1 downto 0);
			SID_FILTER2_NEXT <= WRITE_DATA(5 downto 4);
			-- (3 downto 1) reserved in case we want all revisions!
		end if;

		if (addr_decoded4(7)='1') then
			RESTRICT_CAPABILITY_NEXT(4 downto 0) <= WRITE_DATA(4 downto 0);
		end if;

		if (addr_decoded4(9)='1') then
			CHANNEL_EN_NEXT(4 downto 0) <= WRITE_DATA(4 downto 0);
		end if;

		if (addr_decoded4(12)='1') then
			if (WRITE_DATA=x"3F") then
				CONFIG_ENABLE_NEXT <= '1';
			else
				CONFIG_ENABLE_NEXT <= '0';
			end if;
		end if;		

		if enable_flash=1 then 
			if (addr_decoded4(11)='1') then
				CPU_FLASH_ADDR_NEXT(flash_addr_bits+1 downto 16) <= WRITE_DATA((flash_addr_bits-16)+4 downto 3);

				CPU_FLASH_CFG_NEXT <= WRITE_DATA(2);
				CPU_FLASH_REQUEST_NEXT <= WRITE_DATA(1);
				CPU_FLASH_WRITE_N_NEXT <= WRITE_DATA(0);
			end if;

			if (addr_decoded4(13)='1') then
				CPU_FLASH_ADDR_NEXT(7 downto 0) <= WRITE_DATA;
			end if;

			if (addr_decoded4(14)='1') then
				CPU_FLASH_ADDR_NEXT(15 downto 8) <= WRITE_DATA;
			end if;

			if (addr_decoded4(15)='1') then
				case CPU_FLASH_ADDR_REG(1 downto 0) is
				when "00" =>
					CPU_FLASH_DATA_NEXT(7 downto 0) <= WRITE_DATA;
				when "01" =>
					CPU_FLASH_DATA_NEXT(15 downto 8) <= WRITE_DATA;
				when "10" =>
					CPU_FLASH_DATA_NEXT(23 downto 16) <= WRITE_DATA;
				when others =>
					CPU_FLASH_DATA_NEXT(31 downto 24) <= WRITE_DATA;
				end case;
			end if;
		end if;
	end if;	
end process;

process(addr_decoded4,VERSION_LOC_REG,
SATURATE_REG,CHANNEL_MODE_REG,IRQ_EN_REG,DETECT_RIGHT_REG,
POST_DIVIDE_REG, GTIA_ENABLE_REG,
PSG_FREQ_REG, PSG_STEREOMODE_REG, PSG_PROFILESEL_REG, PSG_ENVELOPE16_REG,
SID_FILTER1_REG, SID_FILTER2_REG,
CPU_FLASH_CFG_REG,CPU_FLASH_ADDR_REG,CPU_FLASH_DATA_REG,
CPU_FLASH_REQUEST_REG, CPU_FLASH_WRITE_N_REG,
RESTRICT_CAPABILITY_REG,
CHANNEL_EN_REG,
PAL_REG
)
	variable ACTUAL_CAPABILITY : std_logic_vector(7 downto 0);
begin
	CONFIG_DO <= (others=>'1');
	
	if (addr_decoded4(0)='1') then
			CONFIG_DO <= (others=>'0');
			CONFIG_DO(0) <= SATURATE_REG;
			CONFIG_DO(2) <= CHANNEL_MODE_REG;
			CONFIG_DO(3) <= IRQ_EN_REG;
			CONFIG_DO(4) <= DETECT_RIGHT_REG;
			CONFIG_DO(5) <= PAL_REG;
	end if;	

	ACTUAL_CAPABILITY := (others=>'0');

	if (pokeys=1) then
		ACTUAL_CAPABILITY(1 downto 0) := "00";
	elsif (pokeys=2) then
		ACTUAL_CAPABILITY(1 downto 0) := "01"; --bit0=stereo
	elsif (pokeys=4) then
		ACTUAL_CAPABILITY(1 downto 0) := "11"; --bit1=quad
	end if;
	if (enable_sid=1) then
		ACTUAL_CAPABILITY(2) := '1';
	else
		ACTUAL_CAPABILITY(2) := '0';
	end if;
	if (enable_psg=1) then
		ACTUAL_CAPABILITY(3) := '1';
	else
		ACTUAL_CAPABILITY(3) := '0';
	end if;		
	if (enable_covox=1) then
		ACTUAL_CAPABILITY(4) := '1';
	else
		ACTUAL_CAPABILITY(4) := '0';
	end if;			
	if (enable_sample=1) then
		ACTUAL_CAPABILITY(5) := '1';
	else
		ACTUAL_CAPABILITY(5) := '0';
	end if;					
	if (enable_flash=1) then
		ACTUAL_CAPABILITY(6) := '1';
	else
		ACTUAL_CAPABILITY(6) := '0';
	end if;					
	
	if (addr_decoded4(1)='1') then
		CONFIG_DO <= ACTUAL_CAPABILITY and "11"&RESTRICT_CAPABILITY_REG(4)&RESTRICT_CAPABILITY_REG;
	end if;
	
	if (addr_decoded4(2)='1') then
		CONFIG_DO <= POST_DIVIDE_REG;
	end if;	
	
	if (addr_decoded4(3)='1') then
		CONFIG_DO <= (others=>'0');
		CONFIG_DO(3 downto 0) <= GTIA_ENABLE_REG;
		--CONFIG_DO(7 downto 4) <= SIO_ENABLE_REG; -- if we implement
	end if;
	
	if (addr_decoded4(4)='1') then
		-- version
		case VERSION_LOC_REG(2 downto 0) is			
			when "000" => 
				CONFIG_DO <= getByte(version,1);
			when "001" =>
				CONFIG_DO <= getByte(version,2);
			when "010" =>
				CONFIG_DO <= getByte(version,3);
			when "011" =>
				CONFIG_DO <= getByte(version,4);
			when "100" => 
				CONFIG_DO <= getByte(version,5);
			when "101" =>
				CONFIG_DO <= getByte(version,6);
			when "110" =>
				CONFIG_DO <= getByte(version,7);
			when "111" =>
				CONFIG_DO <= getByte(version,8);
			when others =>
		end case;		
	end if;

	if (addr_decoded4(5)='1') then
		CONFIG_DO <= (others=>'0');
		CONFIG_DO(1 downto 0) <= PSG_FREQ_REG;
		CONFIG_DO(3 downto 2) <= PSG_STEREOMODE_REG;
		CONFIG_DO(4) <= PSG_ENVELOPE16_REG;
		CONFIG_DO(6 downto 5) <= PSG_PROFILESEL_REG;
	end if;

	if (addr_decoded4(6)='1') then
		CONFIG_DO <= (others=>'0');
		CONFIG_DO(1 downto 0) <= SID_FILTER1_REG;
		-- (3 downto 2) reserved in case we want more filter options
		CONFIG_DO(5 downto 4) <= SID_FILTER2_REG;
		-- (7 downto 6) reserved in case we want more filter options
	end if;

	if (addr_decoded4(7)='1') then
		CONFIG_DO(4 downto 0) <= RESTRICT_CAPABILITY_REG(4 downto 0);
	end if;

	if (addr_decoded4(9)='1') then
		CONFIG_DO(4 downto 0) <= CHANNEL_EN_REG(4 downto 0);
	end if;

	if (addr_decoded4(12)='1') then
		CONFIG_DO <= x"01";
	end if;		

	if enable_flash=1 then 
		if (addr_decoded4(11)='1') then
			CONFIG_DO((flash_addr_bits-16)+4 downto 3) <= CPU_FLASH_ADDR_REG(flash_addr_bits+1 downto 16);
			CONFIG_DO(2) <= CPU_FLASH_CFG_REG;
			CONFIG_DO(1) <= CPU_FLASH_REQUEST_REG;
			CONFIG_DO(0) <= CPU_FLASH_WRITE_N_REG;
		end if;
	
		if (addr_decoded4(13)='1') then
			CONFIG_DO <= CPU_FLASH_ADDR_REG(7 downto 0);
		end if;
	
		if (addr_decoded4(14)='1') then
			CONFIG_DO <= CPU_FLASH_ADDR_REG(15 downto 8);
		end if;
	
		if (addr_decoded4(15)='1') then
			case CPU_FLASH_ADDR_REG(1 downto 0) is
			when "00" =>
				CONFIG_DO <= CPU_FLASH_DATA_REG(7 downto 0);
			when "01" =>
				CONFIG_DO <= CPU_FLASH_DATA_REG(15 downto 8);
			when "10" =>
				CONFIG_DO <= CPU_FLASH_DATA_REG(23 downto 16);
			when others =>
				CONFIG_DO <= CPU_FLASH_DATA_REG(31 downto 24);
			end case;
		end if;
	end if;
	
end process;

end generate;

-------------------------------------------------------
-- AUDIO mixing
mixer1 : entity work.mixer
PORT MAP
(
	CLK => CLK,
	RESET_N => RESET_N,

	ENABLE_CYCLE => ENABLE_CYCLE,

	POST_DIVIDE => POST_DIVIDE_REG,
	DETECT_RIGHT => DETECT_RIGHT_REG,	
	FANCY_ENABLE => FANCY_ENABLE,
	GTIA_EN => GTIA_ENABLE_REG,

	CH0 => POKEY_AUDIO_0,
	CH1 => POKEY_AUDIO_1,
	CH2 => POKEY_AUDIO_2,
	CH3 => POKEY_AUDIO_3,
	CH4 => unsigned(SAMPLE_AUDIO(0)),
	CH5 => unsigned(SAMPLE_AUDIO(1)),
	CH6 => unsigned(SID_AUDIO(0)),
	CH7 => unsigned(SID_AUDIO(1)),	
	CH8 => unsigned(PSG_AUDIO(0)),
	CH9 => unsigned(PSG_AUDIO(1)),		
	CHA(14 downto 12) => (others=>'0'),
	CHA(11) => SIO_RXD_SYNC,
	CHA(10 downto 0) => (others=>'0'),
	CHA(15) => GTIA_AUDIO,			
	
	AUDIO_0_UNSIGNED => AUDIO_0_UNSIGNED,
	AUDIO_1_UNSIGNED => AUDIO_1_UNSIGNED,
	AUDIO_2_UNSIGNED => AUDIO_2_UNSIGNED,
	AUDIO_3_UNSIGNED => AUDIO_3_UNSIGNED
);

--approx line level by using 5V/4 -> ok 1.25V, should be ok approx
dac_0 : entity work.filtered_sigmadelta  --pin37
GENERIC MAP
(
	IMPLEMENTATION => 2,
	LOWPASS => lowpass
)
port map
(
  reset_n => reset_n,
  clk => clk,
  clk2 => CLK116,
  ENABLE_179 => ENABLE_CYCLE,
  audin => AUDIO_0_UNSIGNED,
  AUDOUT => AUDIO_0_SIGMADELTA
);

audout2_on : if enable_audout2=1 generate 

dac_1 : entity work.filtered_sigmadelta
GENERIC MAP
(
	IMPLEMENTATION => 2,
	LOWPASS => lowpass
)
port map
(
  reset_n => reset_n,
  clk => clk,
  clk2 => CLK106,
  ENABLE_179 => ENABLE_CYCLE,
  audin => AUDIO_1_UNSIGNED,
  AUDOUT => AUDIO_1_SIGMADELTA
);

end generate audout2_on;

audout2_off : if enable_audout2=0 generate 
	AUDIO_1_SIGMADELTA <= '0';
end generate audout2_off;

dac_2 : entity work.filtered_sigmadelta
GENERIC MAP
(
	IMPLEMENTATION => 2,
	LOWPASS => lowpass
)
port map
(
  reset_n => reset_n,
  clk => clk,
  clk2 => CLK116,
  ENABLE_179 => ENABLE_CYCLE,
  audin => AUDIO_2_UNSIGNED,
  AUDOUT => AUDIO_2_SIGMADELTA
);

dac_3 : entity work.filtered_sigmadelta
GENERIC MAP
(
	IMPLEMENTATION => 2,
	LOWPASS => lowpass
)
port map
(
  reset_n => reset_n,
  clk => clk,
  clk2 => CLK106,
  ENABLE_179 => ENABLE_CYCLE,
  audin => AUDIO_3_UNSIGNED,
  AUDOUT => AUDIO_3_SIGMADELTA
);

-- Digital audio output
spdif_on : if enable_spdif=1 generate 

-- todo: clock domain crossing!
spdif_mux <= std_logic_vector(audio_2_filtered) when spdif_right='0' 
   else std_logic_vector(audio_3_filtered);

filter_left : entity work.simple_low_pass_filter
PORT MAP 
( 
	CLK => clk,
	AUDIO_IN => audio_2_unsigned,
	SAMPLE_IN => enable_cycle,
	AUDIO_OUT => audio_2_filtered
);

filter_right : entity work.simple_low_pass_filter
PORT MAP 
( 
	CLK => clk,
	AUDIO_IN => audio_3_unsigned,
	SAMPLE_IN => enable_cycle,
	AUDIO_OUT => audio_3_filtered
);

spdif : entity work.spdif_transmitter
 port map(
  bit_clock => CLK6144, -- 128x Fsample (6.144MHz for 48K samplerate)
  data_in(23 downto 8) => spdif_mux,
  data_in(7 downto 0) => (others=>'0'),
  address_out => spdif_right,
  spdif_out => spdif_out
 );

 EXT(SPDIF_BIT) <= spdif_out when CHANNEL_EN_REG(4)='1' else 'Z';
end generate spdif_on;

-- io extension
-- drive to 0 for pot reset (otherwise high imp)
-- drive keyboard lines
	i2c_master0 : entity work.i2c_master
 	generic map(input_clk=>58_000_000, bus_clk=>2_800_000)
	port map(
		clk=>clk,
		reset_n=>reset_n,

		ena=>i2c0_ena,
		addr=>i2c0_addr,
		rw=>i2c0_rw,
		data_wr=>i2c0_write_data,
		busy=>i2c0_busy,
		data_rd=>i2c0_read_data,
		ack_error=>i2c0_error,

		sda=>IOX_SDA,
		scl=>IOX_SCL
	);

	iox_glue : entity work.iox_glue
	port map(
		clk=>clk,
		reset_n=>reset_n,

		ena=>i2c0_ena,
		addr=>i2c0_addr,
		rw=>i2c0_rw,
		write_data=>i2c0_write_data,
		busy=>i2c0_busy,
		read_data=>i2c0_read_data,
		error=>i2c0_error,

		int=>iox_int,

		keyboard_scan=>keyboard_scan,
		keyboard_scan_update=>keyboard_scan_update,
		keyboard_response=>iox_keyboard_response,
		keyboard_scan_enable=>keyboard_scan_enable
	);

-- PS2 keyboard
ps2_on : if enable_ps2=1 generate 
	 PS2CLK <= EXT(PS2CLK_BIT);
	 PS2DAT <= EXT(PS2DAT_BIT);
keyboard_map1 : entity work.ps2_to_atari800
	GENERIC MAP
	(
		ps2_enable => 1,
		direct_enable => 0
	)
	PORT MAP
	( 
		CLK => clk,
		RESET_N => reset_n,
		PS2_CLK => PS2CLK,
		PS2_DAT => PS2DAT, 

		INPUT => open,

		KEY_TYPE => '0', -- TODO 1 is US key_type - probably add editor to pokeycfg an put in flash?
 		ATARI_KEYBOARD_OUT => open,
		
		KEYBOARD_SCAN => KEYBOARD_SCAN,
		KEYBOARD_RESPONSE => PS2_KEYBOARD_RESPONSE,

		CONSOL_START => open,
		CONSOL_SELECT => open,
		CONSOL_OPTION => open,
		
		FKEYS => open,
		FREEZER_ACTIVATE => open,

		PS2_KEYS_NEXT_OUT => open,
		PS2_KEYS => open
	);
	KEYBOARD_RESPONSE <= IOX_KEYBOARD_RESPONSE and PS2_KEYBOARD_RESPONSE;
end generate ps2_on;

ps2_off : if enable_ps2=0 generate 
	KEYBOARD_RESPONSE <= IOX_KEYBOARD_RESPONSE;
end generate ps2_off;


-- Wire up pins
ACLK <= SIO_CLOCKOUT;
BCLK <= '0' when (SIO_CLOCKIN_OE='1' and SIO_CLOCKIN_OUT='0') else 'Z';
SIO_CLOCKIN_IN <= BCLK;

SOD <= '0' when SIO_TXD='0' else 'Z';
SIO_RXD <= SID;
synchronizer_SIO : entity work.synchronizer
	port map (clk=>clk, raw=>SID, sync=>SIO_RXD_SYNC);


--1->pin37
AUD(1) <= AUDIO_0_SIGMADELTA when CHANNEL_EN_REG(0)='1' else '0';

-- ext AUD pins:
AUD(2) <= AUDIO_1_SIGMADELTA when CHANNEL_EN_REG(1)='1' else '0';
AUD(3) <= AUDIO_2_SIGMADELTA when CHANNEL_EN_REG(2)='1' else '0';
AUD(4) <= AUDIO_3_SIGMADELTA when CHANNEL_EN_REG(3)='1' else '0';

IRQ <= '0' when (IRQ_EN_REG='1' and (and_reduce(POKEY_IRQ)='0')) or (IRQ_EN_REG='0' and POKEY_IRQ(0)='0') or (SAMPLE_IRQ='1')  else 'Z';

D <= BUS_DATA when BUS_OE='1' else (others=>'Z');

POTRESET_N <= not(POTRESET) when ext_clk_enable=0 else '1';

END vhdl;
