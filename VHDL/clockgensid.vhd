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

ENTITY clockgensid IS 
	PORT
	(
		CLK : IN STD_LOGIC;
		RESET_N : IN STD_LOGIC;

		PAL : IN STD_LOGIC;
		PHI2 : IN STD_LOGIC; -- C64 1MHz

		MHZ179 : OUT STD_LOGIC; -- C64 method, 1% error
		MHZ358 : OUT STD_LOGIC -- 2x C64 method, 1% error
	);
END clockgensid;		

-- C64 output
--PAL: XL: 2*4.43361875/5, C64: 2*4.43361875/9 -> i.e. XL/C64 = 9/5 -> 2x is 18/5
--NTSC: XL: (315/88)/2, C64: 2*(315/88)/7 -> i.e. XL/C64 = 7/4 -> 2x i 14/4

ARCHITECTURE vhdl OF clockgensid IS
	signal cycle_count_next : unsigned(3 downto 0);
	signal cycle_count_reg : unsigned(3 downto 0);
	signal clk_count_next : unsigned(8 downto 0);
	signal clk_count_reg : unsigned(8 downto 0);
	signal err_next : signed(13 downto 0);
	signal err_reg : signed(13 downto 0);

	signal state_next : std_logic_vector(1 downto 0);
	signal state_reg : std_logic_vector(1 downto 0);
	constant state_sync : std_logic_vector(1 downto 0) := "00";
	constant state_count : std_logic_vector(1 downto 0) := "01";
	constant state_genbegin : std_logic_vector(1 downto 0) := "10";
	constant state_gen : std_logic_vector(1 downto 0) := "11";

	signal everyother_next : std_logic;
	signal everyother_reg : std_logic;
BEGIN
	process(clk,reset_n)
	begin
		if (reset_n='0') then
			cycle_count_reg <= (others=>'0');
			clk_count_reg <= (others=>'0');
			err_reg <= (others=>'0');
			state_reg <= state_sync;
			everyother_reg <= '0';
		elsif (clk'event and clk='1') then
			cycle_count_reg <= cycle_count_next;
			clk_count_reg <= clk_count_next;
			err_reg <= err_next;
			state_reg <= state_next;
			everyother_reg <= everyother_next;
		end if;
	end process;

	process(clk_count_reg,cycle_count_reg,everyother_reg,pal,phi2,state_reg,err_reg)
		variable threshold : unsigned(3 downto 0);
		variable adj1 : unsigned(17 downto 0); 
		variable adj : signed(13 downto 0); -- 8.6 fixed point
		variable trig : std_logic;
		variable cycle_inc : std_logic;
		variable clk_inc : std_logic;
		variable thresh_match : std_logic;

		variable multby : unsigned(8 downto 0);
	begin
		cycle_count_next <= cycle_count_reg;
		clk_count_next <= clk_count_reg;
		state_next <= state_reg;
		err_next <= err_reg;
		everyother_next <= everyother_reg;

		trig := '0';
		cycle_inc := '0';
		clk_inc := '0';

		mhz179 <= '0';
		mhz358 <= '0';

		-- TODO: adjust threshold and multby!
		-- old: pal: threshold=1001, multby = 51  -> 10/9
		-- old: ntsc: threshold=0111, multby = 64 -> 8/7
		if (pal='1') then
			multby := to_unsigned(28,9);
		else
			multby := to_unsigned(36,9);
		end if;
		adj1:= multby * clk_count_reg;
		adj:= signed(adj1(16 downto 3)) - to_signed(64,14);

		--threshold: = "0100"; ntsc
		--threshold: = "0101"; pal
		threshold(0) := pal;
		threshold(1) := '0';
		threshold(2) := '1';
		threshold(3) := '0';

		thresh_match := '0';
		if (cycle_count_reg=threshold) then
			thresh_match := '1';
		end if;

		err_next(13 downto 0) <= err_reg(13 downto 0) + to_signed(64,14);

		case state_reg is
			when state_sync =>
				if (phi2='1') then
					state_next <= state_count;
				end if;
			when state_count =>
				clk_inc := '1';
				if (phi2='1') then
					cycle_inc := '1';
				end if;
				if (thresh_match='1') then
					state_next <= state_gen;
					clk_inc := '0';
				end if;
			when state_genbegin =>
				if (phi2='1') then
					cycle_count_next <= (others=>'0');
					state_next <= state_gen;
					err_next <= -adj;
					trig := '1';
				end if;
			when state_gen =>
				-- threshold here is other size -1, which happens to match!
				if (err_reg(13) = '0') then
					err_next <= err_reg - adj;
					trig := '1';
					cycle_inc := '1';
				end if;
				if (thresh_match='1') then
					state_next <= state_genbegin;
				end if;
			when others=>
				state_next <= state_sync;
		end case;

		if (cycle_inc='1') then
			cycle_count_next <= cycle_count_reg+1;
		end if;

		if (clk_inc='1') then
			clk_count_next <= clk_count_reg + 1;
		end if;

		if (trig='1') then
			mhz358 <= '1';
			everyother_next <= not(everyother_reg);
			mhz179 <= everyother_reg;
		end if;
	end process;


--enable_psg_div2 : entity work.syncreset_enable_divider
--  generic map (COUNT=>29,RESETCOUNT=>6) -- 28-22
--  port map(clk=>clk,syncreset=>'0',reset_n=>reset_n,enable_in=>'1',enable_out=>mhz2);
--
--enable_psg_div1 : entity work.syncreset_enable_divider
--  generic map (COUNT=>58,RESETCOUNT=>6) -- 28-22
--  port map(clk=>clk,syncreset=>'0',reset_n=>reset_n,enable_in=>'1',enable_out=>mhz1);


end vhdl;	
