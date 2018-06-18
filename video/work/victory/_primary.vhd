library verilog;
use verilog.vl_types.all;
entity victory is
    port(
        led             : out    vl_logic_vector(6 downto 0);
        gameover        : in     vl_logic;
        winner          : in     vl_logic
    );
end victory;
