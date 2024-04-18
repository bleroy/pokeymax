LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.numeric_std.all;
use IEEE.STD_LOGIC_MISC.all;

ENTITY slave_timing_6502 IS
	GENERIC (
		address_bits : integer
	);
	PORT (
				CLK: in std_logic;
				RESET_N: in std_logic;
				
				-- input from the cart port
				PHI2 : in std_logic; -- async to our clk (ish):-(
				bus_addr : in std_logic_vector(address_bits-1 downto 0);
				bus_data : in std_logic_vector(7 downto 0);
				bus_cs : in std_logic;
				bus_rw_n : in std_logic;
	
				-- output to the cart port
				bus_data_out : out std_logic_vector(7 downto 0);
				bus_drive : out std_logic;

				-- request for a memory bus cycle (read or write)
				BUS_REQUEST: out std_logic;
				ADDR_IN: out std_logic_vector(address_bits-1 downto 0);
				DATA_IN: out std_logic_vector(7 downto 0);
				RW_N: out std_logic;
				CS : out std_logic;

				ENABLE_CYCLE : out std_logic;
				ENABLE_DOUBLE_CYCLE : out std_logic;

				DATA_OUT: in std_logic_vector(7 downto 0); -- read_data
				DRIVE_DATA_OUT: in std_logic
			);
END slave_timing_6502;

ARCHITECTURE vhdl OF slave_timing_6502 IS

	signal PHI2_sync : std_logic;
	
	signal phi_edge_prev_next : std_logic;
	signal phi_edge_prev_reg: std_logic;	

	signal bus_data_out_next : std_logic_vector(7 downto 0);	
	signal bus_data_out_reg : std_logic_vector(7 downto 0);

	signal bus_drive_next : std_logic;
	signal bus_drive_reg : std_logic;

	signal state_reg : std_logic_vector(1 downto 0);
	signal state_next : std_logic_vector(1 downto 0);
	constant state_wait_addrctl : std_logic_vector(1 downto 0) := "00";
	constant state_wait_addrctl2 : std_logic_vector(1 downto 0) := "01";
	constant state_write_request : std_logic_vector(1 downto 0) := "10";
	constant state_read_output : std_logic_vector(1 downto 0) := "11";

	signal internal_memory_request : std_logic;
	signal registered_read_data_next : std_logic_vector(7 downto 0);
	signal registered_read_data_reg : std_logic_vector(7 downto 0);
	signal registered_drive_bus_next : std_logic;
	signal registered_drive_bus_reg : std_logic;

	signal phi2_falling_edge : std_logic;
	signal phi2_rising_edge : std_logic;
	signal phi2_falling_edge_reg : std_logic;
	signal phi2_either_edge_reg : std_logic;

	signal phi_addr_reg : std_logic_vector(address_bits-1 downto 0);
	signal phi_addr_next : std_logic_vector(address_bits-1 downto 0);
	signal cs_sync : std_logic;
	signal phi_rw_n_reg : std_logic;
	signal phi_rw_n_next : std_logic;
	signal phi_data_reg : std_logic_vector(7 downto 0);
begin
	-- phi2 half, phi2 is in sync properly within the atari, the out of sync stuff is a feature of the PBI buffer I believe
	process(phi2,reset_n)
	begin
		if (reset_n='0') then
			phi_data_reg <= (others=>'0');
		elsif (phi2'event and phi2='0') then -- falling edge
			phi_data_reg <= bus_data;
		end if;
	end process;

	-- Fast half

	process(clk,reset_n)
	begin
		if (reset_n='0') then
			phi_edge_prev_reg <= '1';
			bus_data_out_reg <= (others=>'0');
			bus_drive_reg <= '0';

			registered_read_data_reg <= (others=>'0');
			registered_drive_bus_reg <= '0';

			phi2_falling_edge_reg <= '0';
			phi2_either_edge_reg <= '0';

			state_reg <= state_wait_addrctl;

			phi_addr_reg <= (others=>'0');
			phi_rw_n_reg <= '1';
		elsif (clk'event and clk='1') then
			phi_edge_prev_reg <= phi_edge_prev_next;
			bus_data_out_reg <= bus_data_out_next;
			bus_drive_reg <= bus_drive_next;

			registered_read_data_reg <= registered_read_data_next;
			registered_drive_bus_reg <= registered_drive_bus_next;

			phi2_falling_edge_reg <= phi2_falling_edge;
			phi2_either_edge_reg <= phi2_falling_edge or phi2_rising_edge;

			state_reg <= state_next;

			phi_addr_reg <= phi_addr_next;
			phi_rw_n_reg <= phi_rw_n_next;
		end if;
	end process;
	
	synchronizer_phi : entity work.synchronizer
				 port map (clk=>clk, raw=>PHI2, sync=>PHI2_SYNC);

	synchronizer_cs : entity work.synchronizer
				 port map (clk=>clk, raw=>bus_cs, sync=>cs_sync);

	phi_edge_prev_next <= phi2_sync;

	phi2_falling_edge <= phi_edge_prev_reg and not(phi2_sync);
	phi2_rising_edge <= not(phi_edge_prev_reg) and phi2_sync;

	process(registered_read_data_reg, data_out,
		registered_drive_bus_reg, drive_data_out,
		bus_drive_reg,bus_data_out_reg, 
		phi2_rising_edge,
		phi2_falling_edge,
		phi2_sync,
		cs_sync,
		phi_addr_reg,
		phi_rw_n_reg,
		bus_addr,
		bus_rw_n,
		state_reg)
	begin
		-- maintain snap (only read bus when safe!)
		internal_memory_request <= '0';
		bus_data_out_next <= bus_data_out_reg;
		bus_drive_next <= bus_drive_reg;

		phi_addr_next <= phi_addr_reg;
		phi_rw_n_next <= phi_rw_n_reg;
		registered_read_data_next <= registered_read_data_reg;
		registered_drive_bus_next <= registered_drive_bus_reg;

		-- TODO: Drive off rise/fall of phi2
		-- delays should be based off tsu/th times from data shet
		-- I heard the data from cpu is not always stable on writes at the clock edge, might need to have a short buffer and wait for edge then read etc.

		-- IN:
		-- RW/ADDR tSU (130ns), tH (30ns)
		-- CS tSU (50ns), tH (30ns)
		-- D tSU (130ns), tH (10ns)
		-- OUT:
		-- D tSU (50ns), tH (20ns)
		-- IRQ tDELAY (350ns??) 

		-- write timing
		-- r/w read on rising edge of phi2  -> HELD until falling edge of phi2 + tH
		-- CS read on rising edge of phi2   -> HELD until falling edge of phi2 + tH
		-- addr read on rising edge of phi2 -> HELD until phi2 is risen

		-- read timing
		-- r/w similar
		-- cs similar
		-- drive data tDSR before falling edge - I guess it makes sense to drive it as early after phi2 risen as possible

		-- concern: hold times are known not to be respected, best to read prior to phi2 by some ns
		-- issue with that: we don't know the clock
		-- solution: trigger read/write on phi2 edge, but... use data from a cycle before -> 1 cycle 17ns. 

		-- current solution -> 	
		-- phi2 edge + 13 cycles -> snap addr,rw_n,cs_n -> 232ns
		-- read: then runs pokey, then output from 22->33 cycles -> 392 to 590ns
		-- write: snap bus at cycle 28 -> 500ns
		-- pokey run at 553ns
		-- which is fine for in Atari 8-bit, but not arcade...

		-- since cycle length:
		-- 1.00MHz -> 1000ns
		-- 1.25MHz ->  800ns
		-- 1.50MHz ->  666ns
		-- 1.77MHz ->  564ns
		-- 1.79MHz ->  558ns

		-- solution is to measure

		-- LLLLLLLHHHHHHH
		-- XXAAAAAAAAAAAA
		-- XXXXXXXXXXDDDD
		state_next <= state_reg;
		case (state_reg) is
			when state_wait_addrctl =>
				if (phi2_sync='1' and cs_sync='1') then
					state_next <= state_wait_addrctl2;
					-- snap control signals, should be stable by now
					phi_addr_next <= bus_addr;
					phi_rw_n_next <= bus_rw_n;
				end if;
			when state_wait_addrctl2 =>
				registered_read_data_next <= data_out;
				registered_drive_bus_next <= drive_data_out;
				if (phi_rw_n_reg='1') then -- read
					internal_memory_request <= '1';
					state_next <= state_read_output;
				else
					state_next <= state_write_request;
				end if;
			when state_write_request =>
				if (phi2_falling_edge='1') then
					internal_memory_request <= '1';
					state_next <= state_wait_addrctl;
				end if;
			when state_read_output =>
				bus_data_out_next <= registered_read_data_reg;
				bus_drive_next <= not(phi2_falling_edge) and registered_drive_bus_reg;
				if (phi2_falling_edge='1') then
					state_next <= state_wait_addrctl;
				end if;
			when others =>
		end case;
		
	end process;

	bus_data_out <= bus_data_out_reg;
	bus_drive <= bus_drive_reg;
	bus_request <= internal_memory_request;
	addr_in <= phi_addr_reg;
	data_in <= phi_data_reg;
	rw_n <= phi_rw_n_reg;	
	CS <= cs_sync;

	enable_cycle <= phi2_falling_edge_reg;
	enable_double_cycle <= phi2_either_edge_reg;

end vhdl;
