library verilog;
use verilog.vl_types.all;
entity simple is
    port(
        clk             : in     vl_logic;
        reset           : in     vl_logic;
        w               : in     vl_logic;
        \out\           : out    vl_logic
    );
end simple;
