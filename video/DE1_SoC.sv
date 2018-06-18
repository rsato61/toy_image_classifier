// Top-level module that defines the I/Os for the DE-1 SoC board
module DE1_SoC (CLOCK_50, VGA_R, VGA_G, VGA_B, VGA_BLANK_N, 
						VGA_CLK, VGA_HS, VGA_SYNC_N, VGA_VS, KEY);
	input logic CLOCK_50; // 50MHz clock
	output logic [7:0] VGA_R;
	output logic [7:0] VGA_G;
	output logic [7:0] VGA_B;
	output logic VGA_BLANK_N;
	output logic VGA_CLK;
	output logic VGA_HS;
	output logic VGA_SYNC_N;
	output logic VGA_VS;
	input logic [3:0] KEY;

	// Generate clk off of CLOCK_50, whichClock picks rate.
	logic [31:0] clk;
	logic reset;
	
	assign reset = ~KEY[0];
	
	// 0.75 Hz clock
	parameter whichClock = 25;
	clock_divider cdiv (CLOCK_50, clk);
	
	logic [783:0][7:0] image;
	
	// Make test image
	logic [7:0] valueA = 8'b11111111;
	logic [7:0] valueB = 8'b01111111;
	logic [7:0] valueC = 8'b00111111;
	logic [7:0] valueD = 8'b00011111;
	logic [7:0] valueE = 8'b00001111;
	logic [7:0] valueF = 8'b00000111;
	logic [7:0] valueG = 8'b00000000;
	integer i, j;
	always_comb begin
		for (i = 0; i < 28; i++) begin
			for(j = 0; j < 28; j+=2) begin
				if(i % 2 == 0) begin
					image[(i*28)+j][7:0] = valueA;
					image[(i*28)+j+1][7:0] = valueG;
				end else begin
					image[(i*28)+j][7:0] = valueG;
					image[(i*28)+j+1][7:0] = valueA;
				end
			end
		end
	end
		
	logic clock; // clock 25
	always @(posedge CLOCK_50)
		clock <= ~clock;
	vgaImage vga (image, clock, CLOCK_50, reset, VGA_R, VGA_G, VGA_B, VGA_BLANK_N, VGA_CLK, VGA_HS, VGA_SYNC_N, VGA_VS);

endmodule

// divided_clocks[0] = 25MHz, [1] = 12.5Mhz, ... [23] = 3Hz, [24] = 1.5Hz,
// [25] = 0.75Hz, ...
module clock_divider (clock, divided_clocks);
	input logic clock;
	output logic [31:0] divided_clocks;

	initial begin
		divided_clocks <= 0;
	end

	always_ff @(posedge clock) begin
		divided_clocks <= divided_clocks + 1;
	end

endmodule