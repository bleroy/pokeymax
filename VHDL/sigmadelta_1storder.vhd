---------------------------------------------------------------------------
-- (c) 2020 mark watson
-- I am happy for anyone to use this for non-commercial use.
-- If my vhdl files are used commercially or otherwise sold,
-- please contact me for explicit permission at scrameta (gmail).
-- This applies for source and binary form and derived works.
---------------------------------------------------------------------------
--
-- Simple sigma delta based on https://aip.scitation.org/doi/pdf/10.1063/1.3526240
--
LIBRARY ieee;
USE ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use IEEE.STD_LOGIC_MISC.all;

ENTITY sigmadelta_1storder IS
PORT 
( 
	CLK : IN STD_LOGIC;
	RESET_N : IN STD_LOGIC;

	AUDIN : IN UNSIGNED(15 downto 0);
	AUDOUT : OUT std_logic
);
END sigmadelta_1storder;

ARCHITECTURE vhdl OF sigmadelta_1storder IS	
	signal sigma_latch_next : unsigned(17 downto 0);
	signal sigma_latch_reg : unsigned(17 downto 0);		
	
	signal out_next : std_logic;	
	signal out_reg : std_logic;		
BEGIN

	process(clk,reset_n)
	begin
		if (reset_n='0') then
			sigma_latch_reg <= (others=>'0');
			out_reg <= '0';
		elsif (clk'event and clk='1')  then
			sigma_latch_reg <= sigma_latch_next;
			out_reg <= out_next;
		end if;
	end process;


	process(audin,sigma_latch_reg)
		variable deltab : unsigned(17 downto 0);	
		variable delta_adder_tmp : unsigned(17 downto 0);	
		variable sigma_adder_tmp : unsigned(17 downto 0);	
	begin
		deltab:= (others=>'0');
		deltab(17) := sigma_latch_reg(17);
		deltab(16) := sigma_latch_reg(17); -- not a typo
		delta_adder_tmp := audin + deltab;
		
		sigma_adder_tmp := delta_adder_tmp + sigma_latch_reg;
		
		sigma_latch_next <= sigma_adder_tmp;
		
		out_next <= sigma_latch_reg(17);
	end process;

	audout <= out_reg;
	
end vhdl;

