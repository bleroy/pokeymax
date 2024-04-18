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

LIBRARY work;

-- takes about 5-6 cycles per read (depending on family)
-- So at 116MHz can service all 8 clients at >2MHz - if they all request at once
ENTITY flash_controller IS 
	GENERIC
	(
		addr_bits: integer := 16
	);
	PORT
	(
		CLK : IN STD_LOGIC;
		CLK_SLOW : IN STD_LOGIC;
		RESET_N : IN STD_LOGIC;

		-- Request from device 1 (cpu)
		flash_req1_addr_config : IN STD_LOGIC; -- 1 access config, 0 access main flash
		flash_req1_data_in : IN STD_LOGIC_VECTOR(31 downto 0);
		flash_req1_write_n: IN STD_LOGIC;

		flash_req_request : IN STD_LOGIC_VECTOR(7 downto 0);
		flash_req_complete : OUT STD_LOGIC_VECTOR(7 downto 0);
		flash_req_complete_slow : OUT STD_LOGIC_VECTOR(7 downto 0);

		flash_req1_addr : IN STD_LOGIC_VECTOR(addr_bits-1 downto 0) := (others=>'0');
		flash_req2_addr : IN STD_LOGIC_VECTOR(12 downto 0) := (others=>'0');
		flash_req3_addr : IN STD_LOGIC_VECTOR(12 downto 0) := (others=>'0');
		flash_req4_addr : IN STD_LOGIC_VECTOR(addr_bits-1 downto 0) := (others=>'0');
		flash_req5_addr : IN STD_LOGIC_VECTOR(addr_bits-1 downto 0) := (others=>'0');
		flash_req6_addr : IN STD_LOGIC_VECTOR(12 downto 0) := (others=>'0');
		flash_req7_addr : IN STD_LOGIC_VECTOR(12 downto 0) := (others=>'0');
		flash_req8_addr : IN STD_LOGIC_VECTOR(12 downto 0) := (others=>'0');

		-- Output
		flash_data_out : OUT STD_LOGIC_VECTOR(31 downto 0);
		flash_data_out_slow : OUT STD_LOGIC_VECTOR(31 downto 0)
	);
END flash_controller;		
		
ARCHITECTURE vhdl OF flash_controller IS
	component flash is
	port (
		clock                   : in  std_logic                     := '0';             --    clk.clk
		avmm_csr_addr           : in  std_logic                     := '0';             --    csr.address
		avmm_csr_read           : in  std_logic                     := '0';             --       .read
		avmm_csr_writedata      : in  std_logic_vector(31 downto 0) := (others => '0'); --       .writedata
		avmm_csr_write          : in  std_logic                     := '0';             --       .write
		avmm_csr_readdata       : out std_logic_vector(31 downto 0);                    --       .readdata
		avmm_data_addr          : in  std_logic_vector(addr_bits-1 downto 0) := (others => '0'); --   data.address
		avmm_data_read          : in  std_logic                     := '0';             --       .read
		avmm_data_writedata     : in  std_logic_vector(31 downto 0) := (others => '0'); --       .writedata
		avmm_data_write         : in  std_logic                     := '0';             --       .write
		avmm_data_readdata      : out std_logic_vector(31 downto 0);                    --       .readdata
		avmm_data_waitrequest   : out std_logic;                                        --       .waitrequest
		avmm_data_readdatavalid : out std_logic;                                        --       .readdatavalid
		avmm_data_burstcount    : in  std_logic_vector(1 downto 0)  := (others => '0'); --       .burstcount
		reset_n                 : in  std_logic                     := '0'              -- nreset.reset_n
	);
	end component;

	signal flash_config_addr : std_logic;
	signal flash_config_read : std_logic;
	signal flash_config_di : std_logic_vector(31 downto 0);

	signal flash_config_write : std_logic;
	signal flash_config_do : std_logic_vector(31 downto 0);

	signal flash_data_addr : std_logic_vector(addr_bits-1 downto 0);
	signal flash_data_read : std_logic;
	signal flash_data_di : std_logic_vector(31 downto 0);

	signal flash_data_write : std_logic;
	signal flash_data_do : std_logic_vector(31 downto 0);

	signal flash_data_waitrequest : std_logic;

	signal flash_data_readvalid : std_logic;

	signal flash_data_burstcount : std_logic_vector(1 downto 0);

	signal state_reg : std_logic_vector(3 downto 0);
	signal state_next : std_logic_vector(3 downto 0);
	constant state_idle : std_logic_vector(3 downto 0) := "0000";
	constant state_read : std_logic_vector(3 downto 0) := "0001";
	constant state_write : std_logic_vector(3 downto 0) := "0010";
	constant state_read_wait : std_logic_vector(3 downto 0) := "0011";
	constant state_delay : std_logic_vector(3 downto 0) := "0100";
	constant state_delay2 : std_logic_vector(3 downto 0) := "0101";
	constant state_delay3 : std_logic_vector(3 downto 0) := "0110";
	constant state_delay4 : std_logic_vector(3 downto 0) := "0111";
	constant state_write_wait : std_logic_vector(3 downto 0) := "1000";

	signal request_addr_reg : std_logic_vector(addr_bits-1 downto 0);
	signal request_addr_next : std_logic_vector(addr_bits-1 downto 0);
	signal request_di_reg : std_logic_vector(31 downto 0);
	signal request_di_next : std_logic_vector(31 downto 0);
	signal device_reg : std_logic;
	signal device_next : std_logic;
	signal output_reg : std_logic_vector(7 downto 0);
	signal output_next : std_logic_vector(7 downto 0);

	signal robin_reg : std_logic_vector(7 downto 0);
	signal robin_next : std_logic_vector(7 downto 0);

	signal complete : std_logic;
	signal update_robin : std_logic;
	signal flash_read : std_logic;
	signal flash_readvalid : std_logic;
	signal flash_write_next : std_logic;
	signal flash_write_reg : std_logic;
	signal flash_waitrequest : std_logic;
	signal flash_do : std_logic_vector(31 downto 0);
	
	signal flash_do_next : std_logic_vector(31 downto 0);
	signal flash_do_reg : std_logic_vector(31 downto 0);
	signal flash_complete_next : std_logic_vector(7 downto 0);
	signal flash_complete_reg : std_logic_vector(7 downto 0);	

	signal flash_do_slow_next : std_logic_vector(31 downto 0);
	signal flash_do_slow_reg : std_logic_vector(31 downto 0);
	signal flash_complete_slow_next : std_logic_vector(7 downto 0);
	signal flash_complete_slow_reg : std_logic_vector(7 downto 0);	

BEGIN

	flash1 : flash
	port map
       	(
		clock                   => clk,
		avmm_csr_addr           => request_addr_reg(0),
		avmm_csr_read           => flash_config_read,
		avmm_csr_writedata      => request_di_reg,

		avmm_csr_write          => flash_config_write,
		avmm_csr_readdata       => flash_config_do,

		avmm_data_addr          => request_addr_reg,
		avmm_data_read          => flash_data_read,
		avmm_data_writedata     => request_di_reg,

		avmm_data_write         => flash_data_write,
		avmm_data_readdata      => flash_data_do,

		avmm_data_waitrequest   => flash_data_waitrequest,

		avmm_data_readdatavalid => flash_data_readvalid,

		--avmm_data_burstcount    => "10",
		avmm_data_burstcount    => "01",

		reset_n                 => reset_n
	);

	process(clk,reset_n)
	begin
		if (reset_n='0') then
			state_reg <= state_idle;
			request_addr_reg <= (others=>'0');
			request_di_reg <= (others=>'0');
			device_reg <= '0';
			output_reg <= (others=>'0');
			robin_reg <= "10000000";
			flash_do_reg <= (others=>'0');
			flash_complete_reg <= (others=>'0');
			flash_write_reg <= '0';
		elsif (clk'event and clk='1') then
			state_reg <= state_next;
			request_addr_reg <= request_addr_next;
			request_di_reg <= request_di_next;
			device_reg <= device_next;
			output_reg <= output_next;
			robin_reg <= robin_next;
			flash_do_reg <= flash_do_next;
			flash_complete_reg <= flash_complete_next;
			flash_write_reg <= flash_write_next;
		end if;
	end process;

	process(clk_slow,reset_n)
	begin
		if (reset_n='0') then
			flash_complete_slow_reg <= (others=>'0');
			flash_do_slow_reg <= (others=>'0');
		elsif (clk_slow'event and clk_slow='1') then
			flash_complete_slow_reg <= flash_complete_slow_next;
			flash_do_slow_reg <= flash_do_slow_next;
		end if;
	end process;

	-- state machine
	-- Requests handled round robin
	-- TODO burst!
	process(state_reg,request_addr_reg,request_di_reg,device_reg,output_reg,
		robin_reg,
		flash_req_request,

		flash_req1_addr, flash_req1_data_in, flash_req1_write_n, flash_req1_addr_config,
		flash_req2_addr, flash_req3_addr, flash_req4_addr,
		flash_req5_addr, flash_req6_addr, flash_req7_addr, flash_req8_addr,

		flash_readvalid, flash_waitrequest,

		flash_write_reg,

		complete,
		update_robin
		)
		variable addr : std_logic_vector(addr_bits-1 downto 0);
		variable device : std_logic;
		variable request: std_logic;
	begin
		state_next <= state_reg;
		request_addr_next <= request_addr_reg;
		request_di_next <= request_di_reg;
		device_next <= device_reg;
		output_next <= output_reg;
		robin_next <= robin_reg;

		complete <= '0';
		flash_read <= '0';
		flash_write_next <= flash_write_reg;

		device := '0';
		addr := (others=>'0');
		case robin_reg is
		when x"01" =>
			addr := flash_req1_addr;
			device := flash_req1_addr_config;
		when x"02" =>
			addr(12 downto 0) := flash_req2_addr;
		when x"04" =>
			addr(12 downto 0) := flash_req3_addr;
		when x"08" =>
			addr := flash_req4_addr;
		when x"10" =>
			addr := flash_req5_addr;
		when x"20" =>
			addr(12 downto 0) := flash_req6_addr;
		when x"40" =>
			addr(12 downto 0) := flash_req7_addr;
		when others =>
			addr(12 downto 0) := flash_req8_addr;
		end case;


		update_robin <= complete;
		if (update_robin='1') then
			robin_next <= robin_reg(6 downto 0)&robin_reg(7);
		end if;

		case state_reg is
		when state_idle=>
			if (or_reduce(robin_reg and flash_req_request)='1') then
				request_addr_next <= addr;
				request_di_next <= flash_req1_data_in;

				if (robin_reg(0)='1' and flash_req1_write_n='0') then --write
					state_next <= state_write;
				else
					state_next <= state_read;
				end if;

				output_next <= robin_reg;

				device_next <= device;
			else
				update_robin <= '1';
			end if;
		when state_read=>
			flash_read <= '1';
			state_next <= state_read_wait;
		when state_read_wait =>
			flash_read <= flash_waitrequest;
			if (flash_readvalid = '1') then
				complete <= '1';
				state_next <= state_delay;
			end if;
		when state_write=>
			flash_write_next <= '1';
			state_next <= state_write_wait;
		when state_write_wait=>
			if (flash_waitrequest='0') then
				flash_write_next <= '0';
				complete <= '1';
				state_next <= state_delay;
			end if;
		when state_delay=>
			state_next <= state_delay2; -- client sees complete here, allow client to drop request line before we take next
		when state_delay2=>
			state_next <= state_delay3; -- without a 3rd delay we never get readdatavalid!!
		when state_delay3=>
			state_next <= state_delay4; 
		when state_delay4=>
			if (flash_waitrequest='0') then
				state_next <= state_idle; 
			end if;
		when others=>
			state_next <= state_idle;
		end case;
	end process;

	-- mux on selected device
	process(device_reg, flash_data_do, flash_config_do,
		flash_write_reg,flash_read,flash_data_readvalid,flash_data_waitrequest)
	begin
		flash_do <= (others=>'0');

		flash_config_read <= '0';
		flash_config_write <= '0';
		flash_data_read <= '0';
		flash_data_write <= '0';

		flash_readvalid <= '0';
		flash_waitrequest <= '0';

		if (device_reg='1') then --config
			flash_do <= flash_config_do;
			flash_config_read <= flash_read;
			flash_config_write <= flash_write_reg;
			flash_readvalid <= '1';
		elsif (device_reg='0') then --main
			flash_do <= flash_data_do;
			flash_data_read <= flash_read;
			flash_data_write <= flash_write_reg;
			flash_readvalid <= flash_data_readvalid;
			flash_waitrequest <= flash_data_waitrequest;
		end if;
	end process;

	-- register complete
	flash_complete_next <= output_reg and (complete&complete&complete&complete&complete&complete&complete&complete);
	flash_complete_slow_next <= flash_complete_next or flash_complete_reg;
	process(flash_do_reg,flash_do,complete)
	begin
		flash_do_next <= flash_do_reg;
		flash_do_slow_next <= flash_do_reg;
		if (complete='1') then
			flash_do_next <= flash_do;
			flash_do_slow_next <= flash_do;
		end if;
	end process;

	-- mux on who requested
	flash_req_complete <= flash_complete_reg;
	flash_req_complete_slow <= flash_complete_slow_reg;

	-- outputs
	flash_data_out <= flash_do_reg;
	flash_data_out_slow <= flash_do_slow_reg;

end vhdl;	
