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
use IEEE.STD_LOGIC_MISC.all;

ENTITY stereo_detect IS
PORT 
( 
	CLK : IN STD_LOGIC;
	RESET_N : IN STD_LOGIC;

	A : IN STD_LOGIC; -- raw
	DETECT : OUT STD_LOGIC
);
END stereo_detect;

ARCHITECTURE vhdl OF stereo_detect IS
	signal addr_bit_sync : std_logic;
	signal addr_bit_sync_reg : std_logic;
	signal addr_bit_toggle_count_next : std_logic_vector(1 downto 0);
	signal addr_bit_toggle_count_reg : std_logic_vector(1 downto 0);
BEGIN

        synchronizer_4 : entity work.synchronizer
		port map (clk=>clk, raw=>a, sync=>addr_bit_sync);

	process(clk,reset_n)
	begin
		if (reset_n='0') then
			addr_bit_sync_reg <= '1';
			addr_bit_toggle_count_reg <= (others=>'0');
		elsif (clk'event and clk='1')  then
			addr_bit_sync_reg <= addr_bit_sync;
			addr_bit_toggle_count_reg <= addr_bit_toggle_count_next;
		end if;
	end process;


	process(addr_bit_toggle_count_reg,addr_bit_sync,addr_bit_sync_reg)
	begin
		DETECT <= '0';
		addr_bit_toggle_count_next <= addr_bit_toggle_count_reg;

		if (addr_bit_toggle_count_reg="11") then
			DETECT <= '1';
		else
			if (not(addr_bit_sync = addr_bit_sync_reg)) then
				addr_bit_toggle_count_next <= std_logic_vector(unsigned(addr_bit_toggle_count_reg)+1);
			end if;
		end if;
	end process;

end vhdl;

