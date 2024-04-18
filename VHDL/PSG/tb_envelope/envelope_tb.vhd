library ieee;
  use ieee.std_logic_1164.all;
  use ieee.std_logic_unsigned.all;
  use ieee.numeric_std.all;
  use ieee.std_logic_textio.all;
library std;
  use std.textio.all;

library std_developerskit ; -- used for to_string
--  use std_developerskit.std_iopak.all;

entity envelope_tb is
end;

architecture rtl of envelope_tb is

  constant CLK_PSG_PERIOD : time := 1 us / (10);

  signal clk_psg : std_logic;

  signal reset_n : std_logic;

  signal envelope : std_logic_vector(4 downto 0);

  signal shape : std_logic_vector(3 downto 0);
  signal count_reset : std_logic;

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
	shape <= "0000";
	count_reset <= '1';
	wait for 10us;
	count_reset <= '0';
	wait for 1ms;
	shape <= "0001";
	count_reset <= '1';
	wait for 10us;
	count_reset <= '0';
	wait for 1ms;
	shape <= "0010";
	count_reset <= '1';
	wait for 10us;
	count_reset <= '0';
	wait for 1ms;
	shape <= "0011";
	count_reset <= '1';
	wait for 10us;
	count_reset <= '0';
	wait for 1ms;
	shape <= "0100";
	count_reset <= '1';
	wait for 10us;
	count_reset <= '0';
	wait for 1ms;
	shape <= "0101";
	count_reset <= '1';
	wait for 10us;
	count_reset <= '0';
	wait for 1ms;
	shape <= "0110";
	count_reset <= '1';
	wait for 10us;
	count_reset <= '0';
	wait for 1ms;
	shape <= "0111";
	count_reset <= '1';
	wait for 10us;
	count_reset <= '0';
	wait for 1ms;
	shape <= "1000";
	count_reset <= '1';
	wait for 10us;
	count_reset <= '0';
	wait for 1ms;
	shape <= "1001";
	count_reset <= '1';
	wait for 10us;
	count_reset <= '0';
	wait for 1ms;
	shape <= "1010";
	count_reset <= '1';
	wait for 10us;
	count_reset <= '0';
	wait for 1ms;
	shape <= "1011";
	count_reset <= '1';
	wait for 10us;
	count_reset <= '0';
	wait for 1ms;
	shape <= "1100";
	count_reset <= '1';
	wait for 10us;
	count_reset <= '0';
	wait for 1ms;
	shape <= "1101";
	count_reset <= '1';
	wait for 10us;
	count_reset <= '0';
	wait for 1ms;
	shape <= "1110";
	count_reset <= '1';
	wait for 10us;
	count_reset <= '0';
	wait for 1ms;
	shape <= "1111";
	count_reset <= '1';
	wait for 10us;
	count_reset <= '0';
	wait for 1ms;
end process;


envelope_inst : entity work.PSG_envelope
	PORT MAP
	(
		CLK => clk_psg,
		RESET_N => reset_n,
		ENABLE => '1',
		
		COUNT_RESET => count_reset,
		SHAPE => shape,
		PERIOD => x"000a",
		
		ENVELOPE => envelope
	);

--gtia write process
writing_envelope :
process
    file      outfile  : text is out "envelope.txt";  --declare output file
    variable  outline  : line;   --line number declaration  
begin
wait until clk_psg'event and clk_psg='1';
if(endoffile='0') then   --if the file end is not reached.
write(outline, linenumber, left, 10);
write(outline, to_integer(unsigned(envelope)), left, 4);
write(outline, to_integer(unsigned(shape)), left, 4);
-- write line to external file.
writeline(outfile, outline);
linenumber <= linenumber + 1;
else
null;
end if;

end process writing_envelope;

end rtl;

