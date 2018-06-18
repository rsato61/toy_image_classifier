library verilog;
use verilog.vl_types.all;
entity freds_house_letters is
    port(
        led0            : out    vl_logic_vector(6 downto 0);
        led1            : out    vl_logic_vector(6 downto 0);
        led2            : out    vl_logic_vector(6 downto 0);
        led3            : out    vl_logic_vector(6 downto 0);
        led4            : out    vl_logic_vector(6 downto 0);
        led5            : out    vl_logic_vector(6 downto 0);
        upc             : in     vl_logic_vector(2 downto 0)
    );
end freds_house_letters;
