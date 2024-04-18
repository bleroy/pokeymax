---------------------------------------------------------------------------
-- (c) 2020 mark watson
-- I am happy for anyone to use this for non-commercial use.
-- If my vhdl files are used commercially or otherwise sold,
-- please contact me for explicit permission at scrameta (gmail).
-- This applies for source and binary form and derived works.
---------------------------------------------------------------------------
--
LIBRARY ieee;
USE ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use IEEE.STD_LOGIC_MISC.all;

ENTITY sigmadelta_2ndorder IS
PORT 
( 
	CLK : IN STD_LOGIC;
	RESET_N : IN STD_LOGIC;
	
	ENABLE : IN STD_LOGIC := '1';

	AUDIN : IN UNSIGNED(15 downto 0);
	AUDOUT : OUT std_logic
);
END sigmadelta_2ndorder;

ARCHITECTURE vhdl OF sigmadelta_2ndorder IS	
	signal ttl1_next : signed(21 downto 1);
	signal ttl1_reg : signed(21 downto 1);
	signal ttl2_next : signed(23 downto 1);		
	signal ttl2_reg : signed(23 downto 1);		
	
	signal out_next : std_logic;
	signal out_reg : std_logic;
BEGIN

	process(clk,reset_n)
	begin
		if (reset_n='0') then
			ttl1_reg <= (others=>'0');
			ttl2_reg <= (others=>'0');
			out_reg <= '0';
		elsif (clk'event and clk='1')  then
			ttl1_reg <= ttl1_next;
			ttl2_reg <= ttl2_next;
			out_reg <= out_next;
		end if;
	end process;


--a1=	-2.00000;
--a2=	-10.00000;
--b1=	2.00000;
--c1=	2.00000;
--c2=	1.00000;


	process(audin,out_reg,ttl1_reg,ttl2_reg,enable)
	   variable audinadj : unsigned(16 downto 0);
		variable fb : signed(21 downto 0);	
		variable ttl1_tmp : signed(21 downto 1);
	begin
		out_next <= out_reg;
		ttl1_next <= ttl1_reg;
		ttl2_next <= ttl2_reg;
		
		if (enable='1') then	
			audinadj := resize(audin,17) + to_unsigned(4096,17) - resize(audin(15 downto 3) ,17);
		
			fb:=(others=>'0');
			if (ttl2_reg(23 downto 16)>0) then
				fb(16) := '1';
			else			
				fb(16) := '0';
			end if;
		
			ttl1_tmp := ttl1_reg + resize(signed("0"&audinadj),22-1) - (fb(21-1 downto 0));
			ttl1_next <= ttl1_tmp;

			ttl2_next <= ttl2_reg + resize(((ttl1_tmp(21-1 downto 1)&"0") - ((fb(21-1 downto 0))+(fb(21-3 downto 0)&"00"))),23);	
			
			out_next <= fb(16);	
		end if;
	end process;

	audout <= out_reg;
	
end vhdl;

