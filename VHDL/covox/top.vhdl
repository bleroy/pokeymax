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

ENTITY covox_top IS 
	PORT
	(
		CLK : in std_logic;
		RESET_N : in std_logic;

		WRITE_ENABLE : in std_logic;
		ADDR : in std_logic_vector(1 downto 0);
		DI : in std_logic_vector(7 downto 0);

		DO : out std_logic_vector(7 downto 0);
		AUDIO0 : out std_logic_vector(15 downto 0);
		AUDIO1 : out std_logic_vector(15 downto 0)
	);
END covox_top;		
		
ARCHITECTURE vhdl OF covox_top IS
	signal addr_decoded: std_logic_vector(3 downto 0);
	signal CH1_REG : std_logic_vector(7 downto 0);
	signal CH0_REG : std_logic_vector(7 downto 0);
	signal CH1_NEXT : std_logic_vector(7 downto 0);
	signal CH0_NEXT : std_logic_vector(7 downto 0);
	signal CH3_REG : std_logic_vector(7 downto 0);
	signal CH2_REG : std_logic_vector(7 downto 0);
	signal CH3_NEXT : std_logic_vector(7 downto 0);
	signal CH2_NEXT : std_logic_vector(7 downto 0);

BEGIN
	process(addr_decoded,CH0_REG,CH1_REG,CH2_REG,CH3_REG)
	begin
		DO <= (others=>'0');
	
		if (addr_decoded(0)='1') then
			DO <= CH0_REG;
		end if;
	
		if (addr_decoded(1)='1') then
			DO <= CH1_REG;
		end if;
	
		if (addr_decoded(2)='1') then
			DO <= CH2_REG;
		end if;
	
		if (addr_decoded(3)='1') then
			DO <= CH3_REG;
		end if;
	end process;

	decode_addr2 : entity work.complete_address_decoder
		generic map(width=>2)
		port map (addr_in=>ADDR(1 downto 0), addr_decoded=>addr_decoded);
	
	process(addr_decoded, WRITE_ENABLE,
	CH0_REG,CH1_REG,CH2_REG,CH3_REG,DI)
		variable l : unsigned(8 downto 0);
		variable r : unsigned(8 downto 0);
	begin
		CH0_NEXT <= CH0_REG;
		CH1_NEXT <= CH1_REG;
		CH2_NEXT <= CH2_REG;
		CH3_NEXT <= CH3_REG;
		
		l := resize(unsigned(CH0_REG),9) + resize(unsigned(CH3_REG),9);
		r := resize(unsigned(CH1_REG),9) + resize(unsigned(CH2_REG),9);
		AUDIO0 <= std_logic_vector(l)&"0000000";
		AUDIO1 <= std_logic_vector(r)&"0000000";
	
		
		if (WRITE_ENABLE='1') then
			if (addr_decoded(0)='1') then
				CH0_NEXT <= DI;
			end if;
			if (addr_decoded(1)='1') then
				CH1_NEXT <= DI;
			end if;
			if (addr_decoded(2)='1') then
				CH2_NEXT <= DI;
			end if;
			if (addr_decoded(3)='1') then
				CH3_NEXT <= DI;
			end if;
		end if;
	end process;
	
	process(clk,reset_n)
	begin
		if (reset_n='0') then
			CH0_REG <= (others=>'0');
			CH1_REG <= (others=>'0');
			CH2_REG <= (others=>'0');
			CH3_REG <= (others=>'0');
		elsif (clk'event and clk='1') then
			CH0_REG <= CH0_NEXT;
			CH1_REG <= CH1_NEXT;
			CH2_REG <= CH2_NEXT;
			CH3_REG <= CH3_NEXT;
		end if;
	end process;
END vhdl;
