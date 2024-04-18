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

ENTITY SID_amplitudeModulator IS
PORT 
( 
	CLK : IN STD_LOGIC;
	RESET_N : IN STD_LOGIC;
	ENABLE : IN STD_LOGIC;
	
	WAVE : IN STD_LOGIC_VECTOR(11 downto 0);
	ENVELOPE : IN STD_LOGIC_VECTOR(7 downto 0);
	
	MODULATED : OUT SIGNED(15 downto 0)
);
END SID_amplitudeModulator;

ARCHITECTURE vhdl OF SID_amplitudeModulator IS
	signal mod_reg: signed(15 downto 0);
	signal mod_next: signed(15 downto 0);
BEGIN
	-- register
	process(clk, reset_n)
	begin
		if (reset_n = '0') then
			mod_reg <= (others=>'0');
		elsif (clk'event and clk='1') then
			mod_reg <= mod_next;
		end if;
	end process;
	
	-- next state
	process(mod_reg,enable,wave,envelope)
		variable multres : signed(26 downto 0);
	begin
		mod_next <= mod_reg;
		
		if (enable = '1') then
			multres := signed("0"&envelope)*(signed(resize(unsigned(wave),18))-2048);
			mod_next <= multres(19 downto 4);
		end if;
	end process;	
		
	-- output
	modulated <= mod_reg;
		
END vhdl;
