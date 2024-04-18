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

-- TALK to PCAL6416A io expander over i2c
-- goals: 
-- keyboard:
-- 	set keyboard 6 lines to output
-- 	drive keyboard 6 lines constantly
-- 	receive keyboard 2 lines constantly
-- 	review if we need pull ups on 2 keyboard inputs
-- 	work out which pins in invert on keyboard
-- pot:
-- 	need to drive to 0 to reset
-- 	otherwise need to sit at high impedence input
-- 	so a single bit input is fed in to do this...


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

	POT_RESET : IN STD_LOGIC;
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
	constant state_setup2 : std_logic_vector(3 downto 0) := "0001";
	constant state_setup3 : std_logic_vector(3 downto 0) := "0010";
	constant state_setup4 : std_logic_vector(3 downto 0) := "0011";
	constant state_setup5 : std_logic_vector(3 downto 0) := "0100";
	constant state_setup6 : std_logic_vector(3 downto 0) := "0101";
	--addr,$4f (open drain),00000001 (port 0 is open drain, port 1 is driven)
	--addr,$06 (cfg port0), 00000000 
	--addr,$07 (cfg port1), 00000011 (p1_0,p1_1 are inputs)
	--addr,$47 (pull up/down),00000011 (use pull ups/downs)
	--addr,$49 (pull up/down),00000011 (use pull up 100ks)
	constant state_kbscan : std_logic_vector(3 downto 0) := "0110";
	constant state_kbread : std_logic_vector(3 downto 0) := "0111";
	constant state_potreset : std_logic_vector(3 downto 0) := "1000";
	constant state_setup7 : std_logic_vector(3 downto 0) := "1001";
	-- address, write input port0(02),val
	-- address, read input port1(01),0xff(in)
	-- address, write input port1(03),val

	signal w2 : std_logic;
	signal write1 : std_logic_vector(7 downto 0);
	signal write2 : std_logic_vector(7 downto 0);

	signal i2c_state_reg : std_logic_vector(1 downto 0);
	signal i2c_state_next : std_logic_vector(1 downto 0);
	constant i2c_state_part1 : std_logic_vector(1 downto 0) := "00";
	constant i2c_state_part2 : std_logic_vector(1 downto 0) := "01";
	constant i2c_state_part3 : std_logic_vector(1 downto 0) := "10";

	signal op_complete : std_logic;
	signal busy_reg : std_logic;

	signal pot_next : std_logic;
	signal pot_reg : std_logic;
begin
	process(clk,reset_n)
	begin
		if (reset_n='0') then
			state_reg <= state_setup1;
			i2c_state_reg <= i2c_state_part1;
			busy_reg <= '0';
			pot_reg <= '0';
		elsif (clk'event and clk='1') then
			state_reg <= state_next;
			i2c_state_reg <= i2c_state_next;
			busy_reg <= busy;
			pot_reg <= pot_next;
		end if;
	end process;


	process(i2c_state_reg,w2,write1,write2,busy_reg,busy)
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
					i2c_state_next <= i2c_state_part1;
					op_complete <= '1';
				end if;
			when others =>
				i2c_state_next <= i2c_state_part1;
		end case;
		
	end process;


	process(state_reg,pot_reset,keyboard_scan,busy,busy_reg,read_data,op_complete,pot_reg)
	begin
		state_next <= state_reg;
		pot_next <= pot_reg;

		keyboard_response <= "11";
		keyboard_scan_enable <= '0';

		w2 <= '0';
		write1 <= x"ff";
		write2 <= x"ff";

		case (state_reg) is
			when state_setup1 =>
				w2 <= '1';
				write1 <= x"4f";
				write2 <= "00000001";
				if (op_complete='1') then
					state_next <= state_setup2;
				end if;
			when state_setup2 =>
				w2 <= '1';
				write1 <= x"06";
				write2 <= "00000000";
				if (op_complete='1') then
					state_next <= state_setup3;
				end if;
			when state_setup3 =>
				w2 <= '1';
				write1 <= x"07";
				write2 <= "00000011";
				if (op_complete='1') then
					state_next <= state_setup4;
				end if;
			when state_setup4 =>
				w2 <= '1';
				write1 <= x"47";
				write2 <= "00000011";
				if (op_complete='1') then
					state_next <= state_setup5;
				end if;
			when state_setup5 =>
				w2 <= '1';
				write1 <= x"49";
				write2 <= "00000011";
				if (op_complete='1') then
					state_next <= state_setup6;
				end if;
			when state_setup6 =>
				w2 <= '1';
				write1 <= x"4b";
				write2 <= "11111100";
				if (op_complete='1') then
					state_next <= state_setup7;
				end if;
			when state_setup7 =>
				w2 <= '1';
				write1 <= x"4f";
				write2 <= "00000001";
				if (op_complete='1') then
					state_next <= state_kbscan;
				end if;
			when state_potreset =>
				w2 <= '1';
				write1 <= x"02";
				write2 <= not(pot_reset&pot_reset&pot_reset&pot_reset&pot_reset&pot_reset&pot_reset&pot_reset);
				pot_next <= pot_reset;
				if (op_complete='1') then
					state_next <= state_kbscan;
				end if;
			when state_kbread =>
				w2 <= '0';
				write1 <= x"01";
				write2 <= x"ff";
				if (op_complete='1') then
					keyboard_response <= read_data(0)&read_data(1);
					keyboard_scan_enable <= '1';
					state_next <= state_kbscan;
				end if;
			when state_kbscan =>
				w2 <= '1';
				write1 <= x"03";
				-- Some pokey bits are inverted (k2,k1,k0,k5), handle FPGA side
				--write2 <= not(keyboard_scan(5))&keyboard_scan(4)&keyboard_scan(3)&not(keyboard_scan(0)&keyboard_scan(1)&keyboard_scan(2))&"00";
				write2 <= keyboard_scan(5)&keyboard_scan(4)&keyboard_scan(3)&keyboard_scan(0)&keyboard_scan(1)&keyboard_scan(2)&"00";
				if (op_complete='1') then
					if(not(pot_reset=pot_reg)) then
						state_next <= state_potreset;
					else
						state_next <= state_kbread;
					end if;
				end if;
			when others =>
				state_next <= state_setup1;
		end case;
		
	end process;

end vhdl;


