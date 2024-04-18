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

ENTITY phi_mult IS
PORT 
( 
	clkin : IN STD_LOGIC; -- 55 to 116MHz
	phi2 : IN STD_LOGIC;  -- about 1.8MHz
	clkout : OUT STD_LOGIC -- about 6x phi2, in sync with phi2!
);
END phi_mult;

ARCHITECTURE vhdl OF phi_mult IS
	signal measure_next : std_logic_vector(5 downto 0); -- we do not know osc frequency, measure how many cycles in 1 cycle
	signal measure_reg : std_logic_vector(5 downto 0);

	signal target_next : std_logic_vector(3 downto 0);  -- this is 1/6 of the cycle length, flip our output every time
	signal target_reg : std_logic_vector(3 downto 0);

	signal counter_next : std_logic_vector(3 downto 0);  
	signal counter_reg : std_logic_vector(3 downto 0);	
	
	signal output_next : std_logic;
	signal output_reg : std_logic;

	signal phi2_next : std_logic;
	signal phi2_reg : std_logic;

	signal phi2_sync : std_logic;

	signal flip : std_logic;
begin
	phi2_next <= phi2_sync;
	process(clkin)
	begin
		if (clkin'event and clkin='1') then
			phi2_reg <= phi2_next;
			measure_reg <= measure_next;
			target_reg <= target_next;
			counter_reg <= counter_next;
			output_reg <= output_next;
		end if;
	end process;

	sync_clk : entity work.synchronizer
	port map
	(
		CLK => clkin,
		RAW => phi2,
		SYNC => phi2_sync
	);

	process(phi2_reg, phi2_sync,measure_reg,target_reg,counter_reg)
		variable sum : std_logic_vector(10 downto 0);
	begin
		measure_next <= std_logic_vector(unsigned(measure_reg) + 1);
		target_next <= target_reg;
		counter_next  <= std_logic_vector(unsigned(counter_reg) +1);
		flip <= '0';

		if (phi2_sync = '1' and phi2_reg = '0') then
			measure_next <= "000000";

			-- *21/128 to get 1/6
			-- 10101 is 20...
			sum := std_logic_vector(unsigned("00000"&measure_reg)+unsigned("000"&measure_reg&"00")+unsigned("0"&measure_reg&"0000"));
			target_next <= sum(10 downto 7);
			counter_next <= "0011";
		end if;

		if (counter_reg=target_reg) then
			counter_next <= (others=>'0');
			flip <= '1';
		end if;
	end process;

	process(phi2_reg, phi2_sync, flip, output_reg)
	begin
		output_next <= output_reg;

		if (phi2_sync = '1' and phi2_reg = '0') then
			output_next <= '1'; -- put in sync (should already be one...)
		end if;

		if (flip='1') then
			output_next <= not(output_reg); -- next! 
		end if;
	end process;
	
	CLKOUT <= output_reg;

end vhdl;


