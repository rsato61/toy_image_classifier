library verilog;
use verilog.vl_types.all;
entity tug_of_war_clock_divider is
    port(
        clock           : in     vl_logic;
        divided_clocks  : out    vl_logic_vector(31 downto 0)
    );
end tug_of_war_clock_divider;
