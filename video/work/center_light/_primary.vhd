library verilog;
use verilog.vl_types.all;
entity center_light is
    port(
        clk             : in     vl_logic;
        reset           : in     vl_logic;
        L               : in     vl_logic;
        R               : in     vl_logic;
        NL              : in     vl_logic;
        NR              : in     vl_logic;
        \out\           : out    vl_logic
    );
end center_light;
