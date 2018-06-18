// Modelsim-ASE requires a timescale directive
`timescale 1 ps / 1 ps

// A file to send images to the vgadriver module
// Author: Rebecca Rollins
module vgaImage (image, clk, CLOCK_50, reset, VGA_R, VGA_G, VGA_B, VGA_BLANK_N, VGA_CLK, VGA_HS, VGA_SYNC_N, VGA_VS);
	input logic [783:0][7:0] image;
	input logic clk, CLOCK_50;
	input logic reset;
	output [7:0] VGA_R;
	output [7:0] VGA_G;
	output [7:0] VGA_B;
	output VGA_BLANK_N;
	output VGA_CLK;
	output VGA_HS;
	output VGA_SYNC_N;
	output VGA_VS;
	
	
	logic [9:0] x, oldX;
	logic [8:0] y, oldY;
	logic [9:0] ps, ns; // Pixel location from image
	
	
	// Next state
	always_comb begin
		if(oldX != x) begin
			if(x == 9'b0 && y == 8'b0) begin   // Reached last pixel
				ns = 10'b0;
			end else begin
				ns = (y * 28) + x; //ps[9:0] + 1'b1;
			end
		end else begin
			ns = ps[9:0];
		end
	end
	
	// Set next state for pixel location
	always_ff @(posedge reset or posedge clk) begin
		if(reset)
			ps <= 10'b0;
		else
			ps <= ns;
	end
	
	logic [7:0] value;
	
	// Set old x and y values
	always_ff @(posedge reset or posedge clk) begin
		if(reset) begin
			oldX <= 9'b0;
			oldY <= 8'b0;
		end else begin
			oldX <= x;
			oldY <= y;
		end
	end
	
	assign value = image[ps][7:0];
	
	// Connect to video driver
	video_driver #(.WIDTH(28),.HEIGHT(28)) vgaOut (.CLOCK_50(CLOCK_50), .reset(reset), .x(x), .y(y), 
				.r(value), .g(value), .b(value), 
				.VGA_R(VGA_R), .VGA_G(VGA_G), .VGA_B(VGA_B), 
				.VGA_BLANK_N(VGA_BLANK_N), .VGA_CLK(VGA_CLK), .VGA_HS(VGA_HS),
				.VGA_SYNC_N(VGA_SYNC_N), .VGA_VS(VGA_VS)); 
		
endmodule


module vgaImage_testbench ();
	logic clk, CLOCK_50;
	logic reset;
	logic [7:0] VGA_R;
	logic [7:0] VGA_G;
	logic [7:0] VGA_B;
	logic VGA_BLANK_N;
	logic VGA_CLK;
	logic VGA_HS;
	logic VGA_SYNC_N;
	logic VGA_VS;

	parameter CLOCK_PERIOD=100;
	initial begin
		clk <= 0;
		forever #(CLOCK_PERIOD/2) clk <= ~clk;
	end
	
	logic [783:0][7:0] image;
	
	// Make test image
	logic [7:0] valueA = 8'b00000000;
	logic [7:0] valueB = 8'b00000000;
	integer i;
	always_comb begin
		image[0][7:0] = 8'b11111111;
		image[1][7:0] = valueB;
		for (i = 2; i < 784; i = i + 2) begin
			image[i][7:0] = valueA;
			image[i + 1][7:0] = valueB;
		end
	end

	vgaImage vga (image, clk, clk, reset, VGA_R, VGA_G, 
			VGA_B, VGA_BLANK_N, VGA_CLK, VGA_HS, VGA_SYNC_N, VGA_VS);

	initial begin
		reset <= 1; 	@(posedge clk);
		repeat(5) @(posedge clk);
		reset <= 0; 	@(posedge clk);
		repeat(900) @(posedge clk);
		$stop; // End the simulation.
	end

endmodule
