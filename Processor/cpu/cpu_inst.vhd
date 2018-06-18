	component cpu is
		port (
			clk_clk                   : in  std_logic                     := 'X';             -- clk
			distance_pio_export       : in  std_logic_vector(31 downto 0) := (others => 'X'); -- export
			classification_pio_export : in  std_logic_vector(7 downto 0)  := (others => 'X'); -- export
			read_diff_pio_export      : in  std_logic_vector(7 downto 0)  := (others => 'X'); -- export
			done_read_pio_export      : out std_logic_vector(7 downto 0);                     -- export
			reset_reset_n             : in  std_logic                     := 'X';             -- reset_n
			sdram_clk_clk             : out std_logic                                         -- clk
		);
	end component cpu;

	u0 : component cpu
		port map (
			clk_clk                   => CONNECTED_TO_clk_clk,                   --                clk.clk
			distance_pio_export       => CONNECTED_TO_distance_pio_export,       --       distance_pio.export
			classification_pio_export => CONNECTED_TO_classification_pio_export, -- classification_pio.export
			read_diff_pio_export      => CONNECTED_TO_read_diff_pio_export,      --      read_diff_pio.export
			done_read_pio_export      => CONNECTED_TO_done_read_pio_export,      --      done_read_pio.export
			reset_reset_n             => CONNECTED_TO_reset_reset_n,             --              reset.reset_n
			sdram_clk_clk             => CONNECTED_TO_sdram_clk_clk              --          sdram_clk.clk
		);

