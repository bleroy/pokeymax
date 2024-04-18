library ieee;
 use ieee.std_logic_1164.all;
  use ieee.std_logic_unsigned.all;
  use ieee.numeric_std.all;
  use ieee.std_logic_textio.all;
library std;
  use std.textio.all;

library std_developerskit ; -- used for to_string
--  use std_developerskit.std_iopak.all;

entity top_tb is
end;

architecture rtl of top_tb is

  constant CLK_PSG_PERIOD : time := 1 us / (2); --2MHz

  signal clk_psg : std_logic;

  signal reset_n : std_logic;

  signal addr : std_logic_vector(3 downto 0);
  signal di : std_logic_vector(7 downto 0);
  signal do : std_logic_vector(7 downto 0);
  signal audio1 : std_logic_vector(15 downto 0);
  signal audio2 : std_logic_vector(15 downto 0);
  signal we : std_logic;

  signal    linenumber : integer:=1; 
  signal    endoffile : bit:='0'; 

begin
	p_clk_gen_b : process
	begin
	clk_psg <= '1';
	wait for CLK_PSG_PERIOD/2;
	clk_psg <= '0';
	wait for CLK_PSG_PERIOD - (CLK_PSG_PERIOD/2 );
	end process;

	reset_n <= '0', '1' after 10us;
	--wait until enable_179_early'event and enable_179_early = '1';
	--wait for 100000000us;

process
begin
	wait until reset_n='1';

	addr <= "0000";
	we <= '0';
	wait until (clk_psg'event and clk_psg='0');
	wait until (clk_psg'event and clk_psg='1');

	-- freq
	wait until clk_psg'event and clk_psg='0';
	we <='1';
	addr <= x"0";
	di <= x"ba";
	wait until clk_psg'event and clk_psg='1';
	we <='0';

	wait until clk_psg'event and clk_psg='0';
	we <='1';
	addr <= x"1";
	di <= x"03";
	wait until clk_psg'event and clk_psg='1';
	we <='0';

		--131Hz expected...

	--fixed volume
	wait until clk_psg'event and clk_psg='0';
	we <='1';
	addr <= x"8";
	di <= x"0f"; --fixed vol
	wait until clk_psg'event and clk_psg='1';
	we <='0';

	--all on
	wait until clk_psg'event and clk_psg='0';
	we <='1';
	addr <= x"7";
	di <= x"f8"; --all pure on
	wait until clk_psg'event and clk_psg='1';
	we <='0';

	wait for 10ms;

	-- freq
	wait until clk_psg'event and clk_psg='0';
	we <='1';
	addr <= x"0";
	di <= x"00";
	wait until clk_psg'event and clk_psg='1';
	we <='0';

	wait until clk_psg'event and clk_psg='0';
	we <='1';
	addr <= x"1";
	di <= x"00";
	wait until clk_psg'event and clk_psg='1';
	we <='0';

	-- freq
	wait until clk_psg'event and clk_psg='0';
	we <='1';
	addr <= x"2";
	di <= x"01";
	wait until clk_psg'event and clk_psg='1';
	we <='0';

	wait until clk_psg'event and clk_psg='0';
	we <='1';
	addr <= x"3";
	di <= x"00";
	wait until clk_psg'event and clk_psg='1';
	we <='0';

	-- freq
	wait until clk_psg'event and clk_psg='0';
	we <='1';
	addr <= x"4";
	di <= x"02";
	wait until clk_psg'event and clk_psg='1';
	we <='0';

	wait until clk_psg'event and clk_psg='0';
	we <='1';
	addr <= x"5";
	di <= x"00";
	wait until clk_psg'event and clk_psg='1';
	we <='0';

	--fixed volume
	wait until clk_psg'event and clk_psg='0';
	we <='1';
	addr <= x"8";
	di <= x"0f"; --fixed vol
	wait until clk_psg'event and clk_psg='1';
	we <='0';
	wait until clk_psg'event and clk_psg='0';
	we <='1';
	addr <= x"9";
	di <= x"0f"; --fixed vol
	wait until clk_psg'event and clk_psg='1';
	we <='0';
	wait until clk_psg'event and clk_psg='0';
	we <='1';
	addr <= x"a";
	di <= x"0f"; --fixed vol
	wait until clk_psg'event and clk_psg='1';
	we <='0';

	wait for 2ms;

	-- envelope!
	wait until clk_psg'event and clk_psg='0';
	we <='1';
	addr <= x"0";
	di <= x"ba";
	wait until clk_psg'event and clk_psg='1';
	we <='0';

	wait until clk_psg'event and clk_psg='0';
	we <='1';
	addr <= x"1";
	di <= x"03";
	wait until clk_psg'event and clk_psg='1';
	we <='0';

	wait until clk_psg'event and clk_psg='0';
	we <='1';
	addr <= x"8";
	di <= x"1f"; --envelope vol
	wait until clk_psg'event and clk_psg='1';
	we <='0';
	wait until clk_psg'event and clk_psg='0';
	we <='1';
	addr <= x"9";
	di <= x"00"; --fixed vol
	wait until clk_psg'event and clk_psg='1';
	we <='0';
	wait until clk_psg'event and clk_psg='0';
	we <='1';
	addr <= x"a";
	di <= x"00"; --fixed vol
	wait until clk_psg'event and clk_psg='1';
	we <='0';

	-- envelope freq
	wait until clk_psg'event and clk_psg='0';
	we <='1';
	addr <= x"b";
	di <= x"04"; 
	wait until clk_psg'event and clk_psg='1';
	we <='0';
	wait until clk_psg'event and clk_psg='0';
	we <='1';
	addr <= x"c";
	di <= x"00"; 
	wait until clk_psg'event and clk_psg='1';
	we <='0';

	-- envelope shape
	wait until clk_psg'event and clk_psg='0';
	we <='1';
	addr <= x"d";
	di <= x"0c"; 
	wait until clk_psg'event and clk_psg='1';
	we <='0';

	wait for 50ms;
	

end process;


top_inst : entity work.PSG_top
	PORT MAP
	(
		CLK => clk_psg,
		RESET_N => reset_n,
		ENABLE => '1',

		ENVELOPE32 => '0',
		
		ADDR => addr,
		WRITE_ENABLE => we,
		
		DI => di,
		DO => do,
		
		AUDIO1 => audio1,
		AUDIO2 => audio2
	);

--gtia write process
writing_top :
process
    file      outfile  : text is out "top.txt";  --declare output file
    variable  outline  : line;   --line number declaration  
begin
wait until clk_psg'event and clk_psg='1';
if(endoffile='0') then   --if the file end is not reached.
write(outline, linenumber, left, 10);
write(outline, to_integer(unsigned(audio1)), left, 4);
write(outline, to_integer(unsigned(audio2)), left, 4);
-- write line to external file.
writeline(outfile, outline);
linenumber <= linenumber + 1;
else
null;
end if;

end process writing_top;

end rtl;

