---------------------------------------------------------------------------
-- (c) 2018 mark watson
-- I am happy for anyone to use this for non-commercial use.
-- If my vhdl files are used commercially or otherwise sold,
-- please contact me for explicit permission at scrameta (gmail).
-- This applies for source and binary form and derived works.
---------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.all;
use ieee.numeric_std.all;

-- TALK to PCAL6408A io expander over i2c
-- goals: 
-- keyboard:
-- 	set keyboard 6 lines to output
-- 	drive keyboard 6 lines constantly
-- 	receive keyboard 2 lines constantly
-- 	review if we need pull ups on 2 keyboard inputs
-- 	work out which pins in invert on keyboard


ENTITY iox_glue IS
PORT 
( 
	CLK : IN STD_LOGIC;
	RESET_N : IN STD_LOGIC;

	ENA : OUT STD_LOGIC;
	ADDR : OUT STD_LOGIC_VECTOR(7 downto 1);
	RW : OUT STD_LOGIC;
	WRITE_DATA : OUT STD_LOGIC_VECTOR(7 downto 0);

	BUSY : IN STD_LOGIC;
	READ_DATA : IN STD_LOGIC_VECTOR(7 downto 0);
	ERROR : IN STD_LOGIC;

	INT : IN STD_LOGIC;

	KEYBOARD_SCAN : IN STD_LOGIC_VECTOR(5 downto 0);
	KEYBOARD_RESPONSE : OUT STD_LOGIC_VECTOR(1 downto 0);
	KEYBOARD_SCAN_UPDATE : IN STD_LOGIC;
	KEYBOARD_SCAN_ENABLE : OUT STD_LOGIC
);
END iox_glue;

ARCHITECTURE vhdl OF iox_glue IS
	-- requests to send to i2c
	signal state_reg : std_logic_vector(3 downto 0);
	signal state_next : std_logic_vector(3 downto 0);
	constant state_setup1 : std_logic_vector(3 downto 0) := "0000";
	constant state_setup3 : std_logic_vector(3 downto 0) := "0010";
	constant state_setup4 : std_logic_vector(3 downto 0) := "0011";
	constant state_setup5 : std_logic_vector(3 downto 0) := "0100";
	constant state_setup6 : std_logic_vector(3 downto 0) := "0101";
	--addr,$4f (driven),00000000 (port 1 is driven)
	--addr,$03 (cfg port1), 10000001 (p1_0,p1_7 are inputs)
	--addr,$43 (pull up/down),10000001 (use pull ups/downs)
	--addr,$44 (pull up/down),10000001 (use pull up 100ks)
	constant state_kbscan : std_logic_vector(3 downto 0) := "0110";
	constant state_kbread : std_logic_vector(3 downto 0) := "0111";
	constant state_setup7 : std_logic_vector(3 downto 0) := "1001";
	constant state_kbscan_delay : std_logic_vector(3 downto 0) := "1010";
	-- address, write input port0(02),val
	-- address, read input port1(01),0xff(in)
	-- address, write input port1(03),val

	signal w2 : std_logic;
	signal write1 : std_logic_vector(7 downto 0);
	signal write2 : std_logic_vector(7 downto 0);

	signal i2c_state_reg : std_logic_vector(1 downto 0);
	signal i2c_state_next : std_logic_vector(1 downto 0);
	constant i2c_state_idle : std_logic_vector(1 downto 0) := "00";
	constant i2c_state_part1 : std_logic_vector(1 downto 0) := "01";
	constant i2c_state_part2 : std_logic_vector(1 downto 0) := "10";
	constant i2c_state_part3 : std_logic_vector(1 downto 0) := "11";

	signal op_complete : std_logic;
	signal busy_reg : std_logic;

	signal keyboard_response_reg : std_logic_vector(1 downto 0);
	signal keyboard_response_next : std_logic_vector(1 downto 0);

	signal keyboard_scan_update_pending_reg : std_logic;
	signal keyboard_scan_update_pending_next : std_logic;
	
	signal request : std_logic;
begin
	process(clk,reset_n)
	begin
		if (reset_n='0') then
			state_reg <= state_setup1;
			i2c_state_reg <= i2c_state_idle;
			busy_reg <= '0';
			keyboard_response_reg <= "11";
			keyboard_scan_update_pending_reg <= '0';
		elsif (clk'event and clk='1') then
			state_reg <= state_next;
			i2c_state_reg <= i2c_state_next;
			busy_reg <= busy;
			keyboard_response_reg <= keyboard_response_next;
			keyboard_scan_update_pending_reg <= keyboard_scan_update_pending_next;
		end if;
	end process;


	process(i2c_state_reg,w2,write1,write2,busy_reg,busy,request)
		variable busy_latched : std_logic;
	begin
		i2c_state_next <= i2c_state_reg;

		ena <= '0';
		addr <= "0100000"; -- $40
		rw <= '1';
		write_data <= (others=>'0');
		op_complete <= '0';

		busy_latched := '0';
		if(busy_reg = '0' AND busy = '1') then
			busy_latched := '1';
		end if;

		case (i2c_state_reg) is
			when i2c_state_idle =>
				if (request = '1') then
					i2c_state_next <= i2c_state_part1;
				end if;
			when i2c_state_part1 =>
				ena <= '1';
				rw <= '0';
				write_data <= write1;
				
				if (busy_latched='1') then
					i2c_state_next <= i2c_state_part2;
				end if;
			when i2c_state_part2 =>
				ena <= '1';
				rw <= not(w2);
				write_data <= write2;
				
				if (busy_latched='1') then
					i2c_state_next <= i2c_state_part3;
				end if;
			when i2c_state_part3 =>
				ena <= '0';
				if (busy='0') then
					i2c_state_next <= i2c_state_idle;
					op_complete <= '1';
				end if;
			when others =>
				i2c_state_next <= i2c_state_idle;
		end case;
		
	end process;


	process(state_reg,keyboard_scan,keyboard_scan_update,keyboard_scan_update_pending_reg,busy,busy_reg,read_data,op_complete,int,keyboard_response_reg)
	begin
		state_next <= state_reg;

		keyboard_scan_enable <= '0';
		keyboard_response_next <= keyboard_response_reg;
		keyboard_scan_update_pending_next <= keyboard_scan_update_pending_reg or keyboard_scan_update;

		request <= '0';
		w2 <= '0';
		write1 <= x"ff";
		write2 <= x"ff";

		case (state_reg) is
			when state_setup1 =>
				request <= '1';
				w2 <= '1';
				write1 <= x"4f";
				write2 <= "00000000";
				if (op_complete='1') then
					state_next <= state_setup3;
				end if;
			when state_setup3 =>
				request <= '1';
				w2 <= '1';
				write1 <= x"03";
				write2 <= "10000001";
				if (op_complete='1') then
					state_next <= state_setup4;
				end if;
			when state_setup4 =>
				request <= '1';
				w2 <= '1';
				write1 <= x"43";
				write2 <= "10000001";
				if (op_complete='1') then
					state_next <= state_setup5;
				end if;
			when state_setup5 =>
				request <= '1';
				w2 <= '1';
				write1 <= x"44";
				write2 <= "10000001";
				if (op_complete='1') then
					state_next <= state_setup6;
				end if;
			when state_setup6 =>
				request <= '1';
				w2 <= '1';
				write1 <= x"45";
				write2 <= "01111110";
				if (op_complete='1') then
					state_next <= state_setup7;
				end if;
			when state_setup7 =>
				request <= '1';
				w2 <= '1';
				write1 <= x"4f";
				write2 <= "00000000";
				if (op_complete='1') then
					state_next <= state_kbscan;
				end if;
			when state_kbread =>
				request <= '1';
				w2 <= '0';
				write1 <= x"00";
				write2 <= x"ff";
				if (op_complete='1') then
					keyboard_response_next <= read_data(0)&read_data(7);
					state_next <= state_kbscan_delay;
				end if;

			when state_kbscan_delay =>
				if (int='0') then
					state_next <= state_kbread;
				end if;
				if (keyboard_scan_update_pending_reg='1') then
				   keyboard_scan_update_pending_next <= '0';				
					keyboard_scan_enable <= '1';
					state_next <= state_kbscan;
				end if;


			--	keyboard_scan_enable <= '1';
			--	state_next <= state_kbscan;
			when state_kbscan =>
				request <= '1';
				w2 <= '1';
				write1 <= x"01";
				-- Some pokey bits are inverted (k2,k1,k0,k5), handle FPGA side
				--write2 <= not(keyboard_scan(5))&keyboard_scan(4)&keyboard_scan(3)&not(keyboard_scan(0)&keyboard_scan(1)&keyboard_scan(2))&"00";
				write2 <= "0"&keyboard_scan(0)&keyboard_scan(1)&keyboard_scan(2)&keyboard_scan(3)&keyboard_scan(4)&keyboard_scan(5)&"0";
--v1
--p1_0 - KR2 (act low)
--p1_1 - KR1 (act low)
--p1_2 - K2 (act low)
--p1_3 - k1 (act low)
--p1_4 - k0 (act low)
--p1_5 - k3
--p1_6 - k4
--p1_7 - k5 (act low)

--v2
--P0_0,KR2,
--P0_1,K5,
--P0_2,K4
--P0_3,K3
--P0_4,K2
--P0_5,K1
--P0_6,K0
--P0_7,KR1
				if (op_complete='1') then
					state_next <= state_kbscan_delay;
				end if;
			when others =>
				state_next <= state_setup1;
		end case;
		
	end process;

	keyboard_response <= keyboard_response_reg;

end vhdl;


