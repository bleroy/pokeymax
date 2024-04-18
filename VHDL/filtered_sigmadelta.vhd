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
use IEEE.STD_LOGIC_MISC.all;

ENTITY filtered_sigmadelta IS
GENERIC
(
	implementation : integer :=1; --1:my 1st order, 2:my 2nd order, 3:3rd order (not mine)
	lowpass : integer :=1 -- simple low pass. Was made for HDMI so can be turned off here with little impact to save resources. 
);
PORT 
( 
	CLK : IN STD_LOGIC;
	CLK2 : IN STD_LOGIC;
	RESET_N : IN STD_LOGIC;

	ENABLE_179 : IN STD_LOGIC;

	AUDIN : IN UNSIGNED(15 downto 0);
	AUDOUT : OUT std_logic
);
END filtered_sigmadelta;

ARCHITECTURE vhdl OF filtered_sigmadelta IS	
component hq_dac
port (
  reset :in std_logic;
  clk :in std_logic;
  clk_ena : in std_logic;
  pcm_in : in std_logic_vector(19 downto 0);
  dac_out : out std_logic
);
end component;

	signal AUDIO_FILTERED : unsigned(15 downto 0);
--	
--attribute syn_keep: boolean;
--attribute syn_keep of AUDIO_DIFF13: signal is true;	
--attribute syn_keep of AUDIO_DIFF23: signal is true;	
BEGIN

-- low pass filter output
gen_lowpass_on : if lowpass=1 generate
filter_0 : entity work.simple_low_pass_filter
	port map
	(
		CLK => CLK,
		AUDIO_IN => AUDIN,
		SAMPLE_IN => ENABLE_179,
		AUDIO_OUT => AUDIO_FILTERED
	);
end generate;

gen_lowpass_off : if lowpass=0 generate
AUDIO_FILTERED<=AUDIN;
end generate;

gen_1storder_on : if implementation=1 generate
dac_1st : entity work.sigmadelta_1storder
port map
(
  reset_n => reset_n,
  clk => clk,
  audin => AUDIO_FILTERED,
  AUDOUT => AUDOUT
);
end generate;
	
gen_2ndorder_on : if implementation=2 generate	

dac_2nd : entity work.sigmadelta_2ndorder
port map
(
  reset_n => reset_n,
  clk => clk2,
  audin => AUDIO_FILTERED,
  AUDOUT => AUDOUT
);
end generate;
	
gen_3rdorder_on : if implementation=3 generate	
dac_3rd : hq_dac --Not mine, so can not use I think in the 'release' version
port map
(
  reset => not(reset_n),
  clk => clk,
  clk_ena => '1',
  pcm_in(19 downto 4) => std_logic_vector(AUDIO_FILTERED),
  pcm_in(3 downto 0) => "0000",
  dac_out => AUDOUT
);
end generate;

END vhdl;

