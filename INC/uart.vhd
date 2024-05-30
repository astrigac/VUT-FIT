-- uart.vhd: UART controller - receiving part
-- Author(s): xstrig00
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;

-------------------------------------------------
entity UART_RX is
port(	
    CLK: 	    in std_logic;
	RST: 	    in std_logic;
	DIN: 	    in std_logic;
	DOUT: 	    out std_logic_vector(7 downto 0);
	DOUT_VLD: 	out std_logic := '0'
);
end UART_RX;  

-------------------------------------------------
architecture behavioral of UART_RX is
signal clk_mid      : std_logic := '0';
signal clk_init     : std_logic := '0';
signal clk_cnt      : std_logic_vector(4 downto 0) := "00000";
signal bit_cnt      : std_logic_vector(3 downto 0) := "0000";
signal cnt_e        : std_logic := '0';
signal rd_e         : std_logic := '0';
signal out_vld      : std_logic;
begin
    FSM: entity work.UART_FSM(behavioral)
    port map(
        CLK             => CLK, -- clock
        RST             => RST, -- reset
        DATA             => DIN, -- input data
        CLK_MID         => clk_mid, -- checks mid of bit
        CLK_INIT        => clk_init, -- checks middle of first data bit
        BIT_CNT         => bit_cnt, -- counter for counting bits
        CNT_E           => cnt_e,
        READ_E          => rd_e,
        VALIDITY        => out_vld
    );

    process(CLK) begin
        if rising_edge(CLK) then
            
            DOUT_VLD <= out_vld;
          
            if RST = '1' then
                DOUT <= "00000000";
                bit_cnt <= "0000";
            else
              
                if cnt_e = '1' then
                    clk_cnt <= clk_cnt + 1;
                else
                    clk_cnt <= "00001";
                end if;
                
                if clk_init = '0' and clk_cnt = "00110" then
                    clk_init <= '1';
                    clk_cnt <= "00001";
                end if;
                
                if bit_cnt = "1000" and clk_cnt = "01111" then
                    clk_mid <= '1';
                    clk_cnt <= "00001";
                end if; 
                
                if clk_init = '1' and clk_cnt = "10000" then
                    clk_mid <= '1';
                    clk_cnt <= "00001";
                end if;
                
                if bit_cnt = "1000" and clk_mid = '1' then
                    bit_cnt <= "0000";
                    clk_mid <= '0';
                    clk_init <= '0';
                end if;
                
                if rd_e = '1' and clk_mid = '1' then
                    DOUT(conv_integer(bit_cnt)) <= DIN;
                    bit_cnt <= bit_cnt + 1;
                    clk_mid <= '0';
                end if;
                
            end if;
        end if;
    end process;
end behavioral;
