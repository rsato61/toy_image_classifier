library verilog;
use verilog.vl_types.all;
entity user_inputs is
    port(
        clk             : in     vl_logic;
        reset           : in     vl_logic;
        \out\           : out    vl_logic;
        in_old          : in     vl_logic
    );
end user_inputs;
