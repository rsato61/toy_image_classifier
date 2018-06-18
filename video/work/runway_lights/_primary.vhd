library verilog;
use verilog.vl_types.all;
entity runway_lights is
    port(
        clk             : in     vl_logic;
        reset           : in     vl_logic;
        in0             : in     vl_logic;
        in1             : in     vl_logic;
        \out\           : out    vl_logic_vector(2 downto 0)
    );
end runway_lights;
