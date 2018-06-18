onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate /vgaImage_testbench/vga/clk
add wave -noupdate /vgaImage_testbench/vga/CLOCK_50
add wave -noupdate /vgaImage_testbench/vga/image
add wave -noupdate /vgaImage_testbench/vga/ns
add wave -noupdate /vgaImage_testbench/vga/oldX
add wave -noupdate /vgaImage_testbench/vga/oldY
add wave -noupdate /vgaImage_testbench/vga/ps
add wave -noupdate /vgaImage_testbench/vga/value
add wave -noupdate /vgaImage_testbench/vga/reset
add wave -noupdate /vgaImage_testbench/vga/VGA_B
add wave -noupdate /vgaImage_testbench/vga/VGA_BLANK_N
add wave -noupdate /vgaImage_testbench/vga/VGA_CLK
add wave -noupdate /vgaImage_testbench/vga/VGA_G
add wave -noupdate /vgaImage_testbench/vga/VGA_HS
add wave -noupdate /vgaImage_testbench/vga/VGA_R
add wave -noupdate /vgaImage_testbench/vga/VGA_SYNC_N
add wave -noupdate /vgaImage_testbench/vga/VGA_VS
add wave -noupdate /vgaImage_testbench/vga/x
add wave -noupdate /vgaImage_testbench/vga/y
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {22017 ps} 0}
quietly wave cursor active 1
configure wave -namecolwidth 150
configure wave -valuecolwidth 100
configure wave -justifyvalue left
configure wave -signalnamewidth 1
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 100
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ps
update
WaveRestoreZoom {6540 ps} {52972 ps}
