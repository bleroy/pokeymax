create_clock -period 1.9MHz [get_ports PHI2]

create_clock -period 87.36MHz [get_ports CLK_SLOW]
derive_pll_clocks
derive_clock_uncertainty

set_clock_groups -asynchronous \
  -group { PHI2 } \
  -group { CLK_SLOW } \
  -group { \
    \pll_v2_inst:pll_inst|altpll_component|auto_generated|pll1|clk[0] \
    \pll_v2_inst:pll_inst|altpll_component|auto_generated|pll1|clk[1] \
  } \
  -group { \
    \pll_v2_inst:pll_inst|altpll_component|auto_generated|pll1|clk[2] \
  }

#		IOX_RST : OUT STD_LOGIC;
#		IOX_INT : IN STD_LOGIC;
#		IOX_SDA : INOUT STD_LOGIC;
#		IOX_SCL : INOUT STD_LOGIC

#create_clock -period 56.67MHz -name cart_clk
#set_input_delay -clock cart_clk -max 0.0 [get_ports D[*]]
#set_input_delay -clock cart_clk -min 0.0 [get_ports D[*]] 
#
#set_input_delay -clock cart_clk -max 0.0 [get_ports A[*]]
#set_input_delay -clock cart_clk -min 0.0 [get_ports A[*]] 
#
#set_input_delay -clock cart_clk -max 0.0 [get_ports W_N]
#set_input_delay -clock cart_clk -min 0.0 [get_ports W_N] 
#
#set_input_delay -clock cart_clk -max 0.0 [get_ports CS_COMB]
#set_input_delay -clock cart_clk -min 0.0 [get_ports CS_COMB] 
#
#set_input_delay -clock cart_clk -max 0.0 [get_ports PADDLE]
#set_input_delay -clock cart_clk -min 0.0 [get_ports PADDLE] 
#
#set_input_delay -clock cart_clk -max 0.0 [get_ports IRQ]
#set_input_delay -clock cart_clk -min 0.0 [get_ports IRQ] 
#
#set_input_delay -clock cart_clk -max 0.0 [get_ports SID]
#set_input_delay -clock cart_clk -min 0.0 [get_ports SID] 
#
#set_input_delay -clock cart_clk -max 0.0 [get_ports BCLK]
#set_input_delay -clock cart_clk -min 0.0 [get_ports BCLK] 
#
#set_output_delay -clock cart_clk -max 0.0 [get_ports D[*]]
#set_output_delay -clock cart_clk -min 0.0 [get_ports D[*]] 
#
#set_output_delay -clock cart_clk -max 0.0 [get_ports SOD]
#set_output_delay -clock cart_clk -min 0.0 [get_ports SOD] 
#
#set_output_delay -clock cart_clk -max 0.0 [get_ports ACLK]
#set_output_delay -clock cart_clk -min 0.0 [get_ports ACLK] 
#
#set_output_delay -clock cart_clk -max 0.0 [get_ports BCLK]
#set_output_delay -clock cart_clk -min 0.0 [get_ports BCLK] 
#
#set_output_delay -clock cart_clk -max 0.0 [get_ports AUD[*]]
#set_output_delay -clock cart_clk -min 0.0 [get_ports AUD[*]] 
#
#set_output_delay -clock cart_clk -max 0.0 [get_ports IRQ]
#set_output_delay -clock cart_clk -min 0.0 [get_ports IRQ] 
#
