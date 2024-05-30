-- uart_fsm.vhd: UART controller - finite state machine
-- Author(s): xstrig00
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

-------------------------------------------------
entity UART_FSM is
port(
   CLK              : in    std_logic;
   RST              : in    std_logic;
   DATA             : in    std_logic;
   CLK_MID          : in    std_logic;
   CLK_INIT         : in    std_logic;
   BIT_CNT          : in    std_logic_vector(3 downto 0);
   CNT_E            : out   std_logic;
   READ_E           : out   std_logic;
   VALIDITY         : out   std_logic
   );
end entity UART_FSM;

-------------------------------------------------
architecture behavioral of UART_FSM is
type state_list is (AWAITING_INITIATION, INITIATED, DT_READING, AWAITING_END, DT_VALID);
signal state : state_list := AWAITING_INITIATION;
begin
    CNT_E <= '0' when state = AWAITING_INITIATION or state = DT_VALID else '1';
    READ_E <= '1' when state = DT_READING else '0';
    VALIDITY <= '1' when state = DT_VALID else '0';
    process (CLK) begin
        if rising_edge(CLK) then
            if RST = '1' then
                state <= AWAITING_INITIATION;
            else
                case state is
                when AWAITING_INITIATION => 
                    if DATA = '0' then
                        state <= INITIATED;
                    end if;
                when INITIATED =>
                    if CLK_INIT = '1' then
                        state <= DT_READING;
                    end if;
                when DT_READING =>
                    if BIT_CNT = "1000" then
                        state <= AWAITING_END;
                    end if;
                when AWAITING_END =>
                    if CLK_MID = '1' then
                        state <= DT_VALID;
                    end if;
                when DT_VALID => state <= AWAITING_INITIATION;
                end case;
            end if;
        end if;
    end process;
end behavioral;