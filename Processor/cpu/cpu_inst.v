	cpu u0 (
		.clk_clk                   (<connected-to-clk_clk>),                   //                clk.clk
		.distance_pio_export       (<connected-to-distance_pio_export>),       //       distance_pio.export
		.classification_pio_export (<connected-to-classification_pio_export>), // classification_pio.export
		.read_diff_pio_export      (<connected-to-read_diff_pio_export>),      //      read_diff_pio.export
		.done_read_pio_export      (<connected-to-done_read_pio_export>),      //      done_read_pio.export
		.reset_reset_n             (<connected-to-reset_reset_n>),             //              reset.reset_n
		.sdram_clk_clk             (<connected-to-sdram_clk_clk>)              //          sdram_clk.clk
	);

