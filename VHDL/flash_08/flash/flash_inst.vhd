	component flash is
		port (
			clock                   : in  std_logic                     := 'X';             -- clk
			avmm_csr_addr           : in  std_logic                     := 'X';             -- address
			avmm_csr_read           : in  std_logic                     := 'X';             -- read
			avmm_csr_writedata      : in  std_logic_vector(31 downto 0) := (others => 'X'); -- writedata
			avmm_csr_write          : in  std_logic                     := 'X';             -- write
			avmm_csr_readdata       : out std_logic_vector(31 downto 0);                    -- readdata
			avmm_data_addr          : in  std_logic_vector(15 downto 0) := (others => 'X'); -- address
			avmm_data_read          : in  std_logic                     := 'X';             -- read
			avmm_data_writedata     : in  std_logic_vector(31 downto 0) := (others => 'X'); -- writedata
			avmm_data_write         : in  std_logic                     := 'X';             -- write
			avmm_data_readdata      : out std_logic_vector(31 downto 0);                    -- readdata
			avmm_data_waitrequest   : out std_logic;                                        -- waitrequest
			avmm_data_readdatavalid : out std_logic;                                        -- readdatavalid
			avmm_data_burstcount    : in  std_logic_vector(1 downto 0)  := (others => 'X'); -- burstcount
			reset_n                 : in  std_logic                     := 'X'              -- reset_n
		);
	end component flash;

	u0 : component flash
		port map (
			clock                   => CONNECTED_TO_clock,                   --    clk.clk
			avmm_csr_addr           => CONNECTED_TO_avmm_csr_addr,           --    csr.address
			avmm_csr_read           => CONNECTED_TO_avmm_csr_read,           --       .read
			avmm_csr_writedata      => CONNECTED_TO_avmm_csr_writedata,      --       .writedata
			avmm_csr_write          => CONNECTED_TO_avmm_csr_write,          --       .write
			avmm_csr_readdata       => CONNECTED_TO_avmm_csr_readdata,       --       .readdata
			avmm_data_addr          => CONNECTED_TO_avmm_data_addr,          --   data.address
			avmm_data_read          => CONNECTED_TO_avmm_data_read,          --       .read
			avmm_data_writedata     => CONNECTED_TO_avmm_data_writedata,     --       .writedata
			avmm_data_write         => CONNECTED_TO_avmm_data_write,         --       .write
			avmm_data_readdata      => CONNECTED_TO_avmm_data_readdata,      --       .readdata
			avmm_data_waitrequest   => CONNECTED_TO_avmm_data_waitrequest,   --       .waitrequest
			avmm_data_readdatavalid => CONNECTED_TO_avmm_data_readdatavalid, --       .readdatavalid
			avmm_data_burstcount    => CONNECTED_TO_avmm_data_burstcount,    --       .burstcount
			reset_n                 => CONNECTED_TO_reset_n                  -- nreset.reset_n
		);

