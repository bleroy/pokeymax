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
use work.AudioTypes.all;

LIBRARY work;

ENTITY mixer IS 
PORT
(
	CLK : IN STD_LOGIC;
	RESET_N : IN STD_LOGIC;

	ENABLE_CYCLE : IN STD_LOGIC;

	DETECT_RIGHT : IN STD_LOGIC;
	POST_DIVIDE : IN STD_LOGIC_VECTOR(7 downto 0);
	FANCY_ENABLE : IN STD_LOGIC;
	GTIA_EN : IN STD_LOGIC_VECTOR(3 downto 0);	

	CH0 : IN UNSIGNED(15 downto 0); --pokey0
	CH1 : IN UNSIGNED(15 downto 0); --pokey1
	CH2 : IN UNSIGNED(15 downto 0); --pokey2
	CH3 : IN UNSIGNED(15 downto 0); --pokey3
	CH4 : IN UNSIGNED(15 downto 0); --sample0
	CH5 : IN UNSIGNED(15 downto 0); --sample1
	CH6 : IN UNSIGNED(15 downto 0); --sid0
	CH7 : IN UNSIGNED(15 downto 0); --sid1
	CH8 : IN UNSIGNED(15 downto 0); --psg0
	CH9 : IN UNSIGNED(15 downto 0); --psg1
	CHA : IN UNSIGNED(15 downto 0); --gtia0	

	AUDIO_0_UNSIGNED : out unsigned(15 downto 0);
	AUDIO_1_UNSIGNED : out unsigned(15 downto 0);
	AUDIO_2_UNSIGNED : out unsigned(15 downto 0);
	AUDIO_3_UNSIGNED : out unsigned(15 downto 0)
);
END mixer;		
		
ARCHITECTURE vhdl OF mixer IS
	-- DETECT RIGHT PLAYING
	signal RIGHT_PLAYING_RECENTLY : std_logic;
	signal RIGHT_NEXT : std_logic;
	signal RIGHT_REG : std_logic;
	signal RIGHT_PLAYING_COUNT_NEXT : unsigned(23 downto 0);
	signal RIGHT_PLAYING_COUNT_REG : unsigned(23 downto 0);

	-- sums
	signal audio0_reg : unsigned(15 downto 0);
	signal audio0_next : unsigned(15 downto 0);
	signal audio1_reg : unsigned(15 downto 0);
	signal audio1_next : unsigned(15 downto 0);
	signal audio2_reg : unsigned(15 downto 0);
	signal audio2_next : unsigned(15 downto 0);
	signal audio3_reg : unsigned(15 downto 0);
	signal audio3_next : unsigned(15 downto 0);

	signal acc_reg : unsigned(19 downto 0);
	signal acc_next : unsigned(19 downto 0);
	
	signal secondBatch_reg : std_logic;
	signal secondBatch_next : std_logic;
	
	signal state_reg : unsigned(3 downto 0);
	signal state_next : unsigned(3 downto 0);
	constant state_pokeyA_L : unsigned(3 downto 0) := "0000";
	constant state_pokeyB_L : unsigned(3 downto 0) := "0001";
	constant state_sample_L : unsigned(3 downto 0) := "0010";
	constant state_sid_L    : unsigned(3 downto 0) := "0011";
	constant state_psg_L    : unsigned(3 downto 0) := "0100";
	constant state_gtia_L   : unsigned(3 downto 0) := "0101";
	constant state_clear_L  : unsigned(3 downto 0) := "0110";
	constant state_pokeyA_R : unsigned(3 downto 0) := "0111";
	constant state_pokeyB_R : unsigned(3 downto 0) := "1000";
	constant state_sample_R : unsigned(3 downto 0) := "1001";
	constant state_sid_R    : unsigned(3 downto 0) := "1010";
	constant state_psg_R    : unsigned(3 downto 0) := "1011";
	constant state_gtia_R   : unsigned(3 downto 0) := "1100";
	constant state_clear_R  : unsigned(3 downto 0) := "1101";
	
	signal channelsel : std_logic_vector(3 downto 0);
	
	signal volume : unsigned(15 downto 0);
	signal saturated : unsigned(15 downto 0);
	
	signal ready : std_logic;
	
	signal write_0 : std_logic;
	signal write_1 : std_logic;
	signal write_2 : std_logic;
	signal write_3 : std_logic;
BEGIN
-- DETECT IF RIGHT CHANNEL PLAYING
-- TODO: into another entity
process(clk,reset_n)
begin
	if (reset_n='0') then
		RIGHT_REG <= '0';
		RIGHT_PLAYING_COUNT_REG <= (others=>'0');
		audio0_reg <= (others=>'0');
		audio1_reg <= (others=>'0');
		audio2_reg <= (others=>'0');
		audio3_reg <= (others=>'0');
		acc_reg <= (others=>'0');
		secondBatch_reg <= '0';
		state_reg <= state_pokeyA_L;
	elsif (clk'event and clk='1') then
		RIGHT_REG <= RIGHT_NEXT;
		RIGHT_PLAYING_COUNT_REG <= RIGHT_PLAYING_COUNT_NEXT;
		audio0_reg <= audio0_next;
		audio1_reg <= audio1_next;
		audio2_reg <= audio2_next;
		audio3_reg <= audio3_next;
		acc_reg <= acc_next;
		secondBatch_reg <= secondBatch_next;
		state_reg <= state_next;
	end if;
end process;

process(RIGHT_NEXT,RIGHT_REG,ENABLE_CYCLE,RIGHT_PLAYING_RECENTLY,RIGHT_PLAYING_COUNT_REG)
begin
	RIGHT_PLAYING_COUNT_NEXT <= RIGHT_PLAYING_COUNT_REG;

	if (ENABLE_CYCLE='1' and RIGHT_PLAYING_RECENTLY='1') then
		RIGHT_PLAYING_COUNT_NEXT <= RIGHT_PLAYING_COUNT_REG-1;
	end if;

	if (RIGHT_NEXT/=RIGHT_REG) then
		RIGHT_PLAYING_COUNT_NEXT <= (others=>'1');
	end if;
end process;
RIGHT_PLAYING_RECENTLY <= or_reduce(std_logic_vector(RIGHT_PLAYING_COUNT_REG));


	process(state_reg,secondBatch_reg,acc_reg,volume,ready,
	POST_DIVIDE,FANCY_ENABLE,RIGHT_PLAYING_RECENTLY,DETECT_RIGHT,RIGHT_REG,SATURATED,GTIA_EN)
		variable postdivide : std_logic_vector(1 downto 0);
		variable presaturate : unsigned(19 downto 0);
		variable leftOnRight : std_logic;
		variable clearAcc : std_logic;
		variable R : std_logic;
		variable pdsel : std_logic_vector(1 downto 0);
	begin
		state_next <= state_reg;
		secondBatch_next <= secondBatch_reg;
		acc_next <= acc_reg;
		RIGHT_NEXT <= RIGHT_REG;
		
		leftOnRight := not(FANCY_ENABLE) or (not(RIGHT_PLAYING_RECENTLY) AND DETECT_RIGHT);		
		clearAcc := '0';

		write_0 <= '0';
		write_1 <= '0';
		write_2 <= '0';
		write_3 <= '0';
		R := '0';
		channelsel <= (others=>'0');
		saturated <= (others=>'0');
		
		if (ready='1') then		
			case state_reg is
				when state_pokeyA_L =>
					channelsel <= x"0";					
					state_next <= state_pokeyB_L;				
				when state_pokeyB_L =>
					channelsel <= x"2";
					state_next <= state_sample_L;
				when state_sample_L =>
					channelsel <= x"4";
					state_next <= state_sid_L;	
				when state_sid_L    =>
					channelsel <= x"6";
					state_next <= state_psg_L;
				when state_psg_L    =>
					channelsel <= x"8";
					state_next <= state_gtia_L;
				when state_gtia_L   =>
					channelsel <= x"a";
					write_0 <= not(gtia_en(0));	
					write_1 <= not(gtia_en(0)) and leftOnRight;	
					write_2 <= not(gtia_en(2));	
					write_3 <= not(gtia_en(2)) and leftOnRight; 						
					state_next <= state_clear_L;
				when state_clear_L   =>
					clearAcc := '1';
					write_0 <= gtia_en(0);	
					write_1 <= gtia_en(0) and leftOnRight;	
					write_2 <= gtia_en(2);	
					write_3 <= gtia_en(2) and leftOnRight;				
					state_next <= state_pokeyA_R;
				when state_pokeyA_R =>
					channelsel <= x"1";
					R := '1';
					state_next <= state_pokeyB_R;
				when state_pokeyB_R =>
					channelsel <= x"3";
					R := '1';
					state_next <= state_sample_R;
				when state_sample_R =>
					channelsel <= x"5";
					R := '1';
					state_next <= state_sid_R;
				when state_sid_R    =>
					channelsel <= x"7";
					R := '1';
					state_next <= state_psg_R;
				when state_psg_R    =>
					channelsel <= x"9";
					R := '1';
					state_next <= state_gtia_R;
				when state_gtia_R   =>
					channelsel <= x"b";
					R := '1';
					write_1 <= not(gtia_en(1)) and not(leftOnRight);
					write_3 <= not(gtia_en(3)) and not(leftOnRight);	
					-- NEEDS DOING WITHOUT GTIA mixed, since that plays on all channels!!
					RIGHT_NEXT <= xor_reduce(std_logic_vector(saturated));						
					state_next <= state_clear_R;
				when state_clear_R   =>
					clearAcc := '1';
					R := '1';
					write_1 <= gtia_en(1) and not(leftOnRight);
					write_3 <= gtia_en(3) and not(leftOnRight);				
					state_next <= state_pokeyA_L;		
					secondBatch_next <= not(secondBatch_reg);
				when others =>
					state_next <= state_pokeyA_L;
			end case;

			pdsel(1) := secondBatch_reg;
			pdsel(0) := R;
			case pdsel is 
			when "00" =>
				postdivide:=POST_DIVIDE(1 downto 0);
			when "01" =>
				postdivide:=POST_DIVIDE(3 downto 2);
			when "10" =>
				postdivide:=POST_DIVIDE(5 downto 4);
			when "11" =>
				postdivide:=POST_DIVIDE(7 downto 6);
			when others =>
			end case;
			
			case postdivide is
				when "00" =>
					presaturate := resize(acc_reg(19 downto 0),20);
				when "01" =>
					presaturate := resize(acc_reg(19 downto 1),20);
				when "10" =>
					presaturate := resize(acc_reg(19 downto 2),20);
				when "11" =>
					presaturate := resize(acc_reg(19 downto 3),20);
				when others =>
			end case;	
			if or_reduce(std_logic_vector(presaturate(19 downto 16)))='1' then
				saturated <= (others=>'1');
			else
				saturated <= presaturate(15 downto 0);
			end if;					

			if (clearAcc='1') then
				acc_next <= (others=>'0');
			else
				acc_next <= acc_reg + resize(volume,19);
			end if;
		end if;

	end process;		
	
	process(state_reg,channelsel,
		ch0,ch1,ch2,ch3,ch4,ch5,ch6,ch7,ch8,ch9,cha
		)
	begin
		volume <= (others=>'0');
		ready <= '1';	
		case channelsel is
		when x"0" =>
		        volume <= ch0;
		when x"1" =>
		        volume <= ch1;
		when x"2" =>
		        volume <= ch2;
		when x"3" =>
		        volume <= ch3;
		when x"4" =>
		        volume <= ch4;
		when x"5" =>
		        volume <= ch5;
		when x"6" =>
			volume <= ch6;
		when x"7" =>
			volume <= ch7;
		when x"8" =>
			volume <= ch8;
		when x"9" =>
			volume <= ch9;
		when x"a" =>
		        volume <= cha;
		when others =>
		end case;
	end process;
	
	process(saturated,secondBatch_reg,write_0,write_1,write_2,write_3,audio0_reg,audio1_reg,audio2_reg,audio3_reg)
	begin
		audio0_next <= audio0_reg;
		audio1_next <= audio1_reg;
		audio2_next <= audio2_reg;
		audio3_next <= audio3_reg;
		
		if (write_0='1' and secondBatch_reg='0') then
			audio0_next <= saturated;
		end if;
		if (write_1='1' and secondBatch_reg='0') then
			audio1_next <= saturated;
		end if;
		if (write_2='1' and secondBatch_reg='1') then
			audio2_next <= saturated;
		end if;
		if (write_3='1' and secondBatch_reg='1') then
			audio3_next <= saturated;
		end if;		
	end process;	
	
-- output
	AUDIO_0_UNSIGNED <= audio0_reg;
	AUDIO_1_UNSIGNED <= audio1_reg;
	AUDIO_2_UNSIGNED <= audio2_reg;
	AUDIO_3_UNSIGNED <= audio3_reg;
end vhdl;
