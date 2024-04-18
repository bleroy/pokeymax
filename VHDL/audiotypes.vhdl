LIBRARY ieee;
USE ieee.std_logic_1164.all;

package AudioTypes is

type SID_AUDIO_TYPE is array(NATURAL range<>) of std_logic_vector(15 downto 0);
type PSG_AUDIO_TYPE is array(NATURAL range<>) of std_logic_vector(15 downto 0);
type PSG_CHANNEL_TYPE is array(NATURAL range<>) of std_logic_vector(4 downto 0);
type POKEY_AUDIO is array(NATURAL range<>) of std_logic_vector(3 downto 0);
type SAMPLE_AUDIO_TYPE is array(NATURAL range<>) of std_logic_vector(15 downto 0);

end package AudioTypes;
