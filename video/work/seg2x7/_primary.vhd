library verilog;
use verilog.vl_types.all;
entity seg2x7 is
    port(
        HEX0            : out    vl_logic_vector(6 downto 0);
        HEX1            : out    vl_logic_vector(6 downto 0);
        SW              : in     vl_logic_vector(9 downto 0)
    );
end seg2x7;
