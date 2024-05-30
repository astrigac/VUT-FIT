-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2023 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Aurel Strigac <xstrig00@stud.fit.vutbr.cz>
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic;                      -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'

   -- stavove signaly
   READY    : out std_logic;                      -- hodnota 1 znamena, ze byl procesor inicializovan a zacina vykonavat program
   DONE     : out std_logic                       -- hodnota 1 znamena, ze procesor ukoncil vykonavani programu (narazil na instrukci halt)
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is
  type fsm_states is (
    sstart,

    sinit_fetch,
    sinit_decode,

    sfetch,
    sdecode,

    sptr_inc,
    sptr_dec,

    sval_inc1,
    sval_inc2,

    sval_dec1,
    sval_dec2,

    sread1,
    sread2,

    swrite1,
    swrite2,

    swhile_start1,
    swhile_start2,
    swhile_start3,
    swhile_start4,

    sbreak1,
    sbreak2,
    sbreak3,
    sbreak4,

    swhile_end1,
    swhile_end2,
    swhile_end3,
    swhile_end4,
    swhile_end5,

    signore,
    shalt
  );

  signal state  : fsm_states;
  signal nstate : fsm_states;

  --pc
  signal pc_reg : std_logic_vector(12 downto 0); 
  signal pc_inc : std_logic;
  signal pc_dec : std_logic;

  --ptr
  signal ptr_reg : std_logic_vector(12 downto 0);
  signal ptr_inc : std_logic;
  signal ptr_dec : std_logic;

  --cnt
  signal cnt_reg : std_logic_vector(7 downto 0);
  signal cnt_inc : std_logic;
  signal cnt_dec : std_logic;

  --mx1
  signal mx1_sel : std_logic; 

  --mx2
  signal mx2_sel : std_logic_vector(1 downto 0);
begin

  cnt_reg_proc : process (CLK, RESET)
  begin
    if (RESET = '1') then
      cnt_reg <= (others => '0');
    elsif (rising_edge(CLK)) then
      if (cnt_inc = '1') then
        cnt_reg <= cnt_reg + 1;
      elsif (cnt_dec = '1') then
        cnt_reg <= cnt_reg - 1;
      end if;
    end if;
  end process;

  ptr_reg_proc : process (CLK, RESET)
  begin
    if (RESET = '1') then
      ptr_reg <= (others => '0');
    elsif (rising_edge(CLK)) then
      if (ptr_inc = '1') then
        if ptr_reg = "1111111111111" then
          ptr_reg <= "0000000000000";
        else
          ptr_reg <= ptr_reg + 1;
          end if;
      elsif (ptr_dec = '1') then
          if ptr_reg = "0000000000000" then
            ptr_reg <= "1111111111111";
          else
            ptr_reg <= ptr_reg - 1;
        end if;
      end if;
    end if;
  end process;

  pc_reg_proc : process (CLK, RESET)
  begin
    if (RESET = '1') then
      pc_reg <= (others => '0');
    elsif (rising_edge(CLK)) then
      if (pc_inc = '1') then
        pc_reg <= pc_reg + 1;
      elsif (pc_dec = '1') then
        pc_reg <= pc_reg - 1;
      end if;
    end if;
  end process;

  mx1_proc : process (mx1_sel, pc_reg, ptr_reg) 
  begin
    if(mx1_sel = '0') then
      DATA_ADDR <= ptr_reg;
    else
      DATA_ADDR <= pc_reg;
    end if;
  end process;

  mx2_proc : process (mx2_sel, DATA_RDATA, IN_DATA)
  begin
    case mx2_sel is
      when "00" => DATA_WDATA <= IN_DATA; 
      when "01" => DATA_WDATA <= DATA_RDATA - 1;
      when "10" => DATA_WDATA <= DATA_RDATA + 1;
      when others => DATA_WDATA <= DATA_RDATA;
    end case;
  end process;
  
  --present state process
  pstate_proc: process (RESET, CLK, EN)
  begin
    if RESET = '1' then
      state <= sstart;
    elsif (rising_edge(CLK)) then
      if EN = '1' then
        state <= nstate;
      end if;
    end if;
  end process;
  
  -- next state process
  nstate_proc: process (state, IN_VLD, OUT_BUSY, DATA_RDATA)
  begin
    nstate <= sstart;
    pc_inc <= '0';
    pc_dec <= '0';
    ptr_inc <= '0';
    ptr_dec <= '0';
    cnt_inc <= '0';
    cnt_dec <= '0';
    mx1_sel <= '0';
    mx2_sel <= "00";
    OUT_WE <= '0';
    IN_REQ <= '0';
    DATA_EN <= '0';
    DATA_RDWR <= '0';
    DONE <= '0';

    case state is

      -----------------------------------------------------------
      when sstart =>
        READY <= '0';
        nstate <= sinit_fetch;
          
      when sinit_fetch =>
        DATA_EN <= '1';
        nstate <= sinit_decode;
      
      when sinit_decode =>
        ptr_inc <= '1';
        if DATA_RDATA = X"40" then
          READY <= '1';
          nstate <= sfetch;
        else
          nstate <= sinit_fetch;
        end if;
      -----------------------------------------------------------

      when sfetch =>
        DATA_EN <= '1';
        mx1_sel <= '1';
        nstate <= sdecode;

      when sdecode =>
        case DATA_RDATA is
          when X"2B" => nstate <= sval_inc1; -- +
          when X"2C" => nstate <= sread1; -- ,
          when X"2D" => nstate <= sval_dec1; -- -
          when X"2E" => nstate <= swrite1; -- .
          when X"3C" => nstate <= sptr_dec; -- <
          when X"3E" => nstate <= sptr_inc; -- >
          when X"40" => nstate <= shalt; -- @
          when X"5B" => nstate <= swhile_start1; -- [
          when X"5D" => nstate <= swhile_end1; -- ]
          when X"7E" => nstate <= sbreak1; -- ~
          when others => nstate <= signore; -- ignore
        end case;
      
      -----------------------------------------------------------

      when sptr_inc =>
        ptr_inc <= '1';
        pc_inc <= '1';
        nstate <= sfetch;
      
      -----------------------------------------------------------

      when sptr_dec =>
        ptr_dec <= '1';
        pc_inc <= '1';
        nstate <= sfetch;
      
      -----------------------------------------------------------

      when sval_inc1 =>
        DATA_EN <= '1';
        nstate <= sval_inc2;
      
      when sval_inc2 =>
        DATA_EN <= '1';
        DATA_RDWR <= '1';
        mx2_sel <= "10";
        pc_inc <= '1';
        nstate <= sfetch;

      ----------------------------------------------------------- 
      
      when sval_dec1 =>
        DATA_EN <= '1';
        nstate <= sval_dec2;
      
      when sval_dec2 =>
        DATA_EN <= '1';
        DATA_RDWR <= '1';
        mx2_sel <= "01";
        pc_inc <= '1';
        nstate <= sfetch;
      
      -----------------------------------------------------------  

      when sread1 =>
        DATA_EN <= '1';
        IN_REQ <= '1';
        nstate <= sread2;

      when sread2 =>
        if IN_VLD = '1' then
          DATA_EN <= '1';
          DATA_RDWR <= '1';
          pc_inc <= '1';
          nstate <= sfetch;
        else
          nstate <= sread1;
        end if;

      -----------------------------------------------------------  

      when swrite1 =>
        DATA_EN <= '1';
        nstate <= swrite2;

      when swrite2 =>
        if OUT_BUSY = '0' then
          OUT_WE <= '1';
          OUT_DATA <= DATA_RDATA;
          pc_inc <= '1';
          nstate <= sfetch;
        else
          nstate <= swrite1;
        end if;
      
      -----------------------------------------------------------  

      when swhile_start1 =>
        DATA_EN <= '1';
        pc_inc <= '1';
        nstate <= swhile_start2;

      when swhile_start2 =>
        if DATA_RDATA = "00000000" then
          cnt_inc <= '1';
          nstate <= swhile_start3;
        else
          nstate <= sfetch;
        end if;

      when swhile_start3 =>
        if cnt_reg = "00000000" then
          nstate <= sfetch;
        else
          DATA_EN <= '1';
          mx1_sel <= '1';
          nstate <= swhile_start4;
        end if ;
        
      when swhile_start4 =>
        pc_inc <= '1';
        if DATA_RDATA = X"5B" then
          cnt_inc <= '1';
        elsif DATA_RDATA = X"5D" then
          cnt_dec <= '1';
        end if ;
        nstate <= swhile_start3;

      -----------------------------------------------------------  

      when sbreak1 =>
        pc_inc <= '1';
        cnt_inc <= '1';
        nstate <= sbreak2;

      when sbreak2 =>
        if cnt_reg = "00000000" then
          nstate <= sfetch;
        else
          DATA_EN <= '1';
          mx1_sel <= '1';
          nstate <= sbreak3;
        end if ;
      
      when sbreak3 =>
        pc_inc <= '1';
        if DATA_RDATA = X"5B" then
          cnt_inc <= '1';
        elsif DATA_RDATA = X"5D" then
          cnt_dec <= '1';
        end if ;
        nstate <= sbreak2;
      
      ----------------------------------------------------------

      when swhile_end1 =>
        DATA_EN <= '1';
        nstate <= swhile_end2;
      
      when swhile_end2 =>
        if DATA_RDATA = "00000000" then
          pc_inc <= '1';
          nstate <= sfetch;
        else
          cnt_inc <= '1';
          pc_dec <= '1';
          nstate <= swhile_end3;
        end if;

      when swhile_end3 =>
        if cnt_reg <= "00000000" then
          nstate <= sfetch;
        else
          DATA_EN <= '1';
          mx1_sel <= '1';
          nstate <= swhile_end4;
        end if;

      when swhile_end4 =>
        if DATA_RDATA = X"5B" then
          cnt_dec <= '1';
        elsif DATA_RDATA = X"5D" then
          cnt_inc <= '1';
        end if;
        nstate <= swhile_end5;

      when swhile_end5 =>
        DATA_EN <= '1';
        if cnt_reg = "00000000" then
          pc_inc <= '1';
        else
          pc_dec <= '1';
        end if;
        nstate <= swhile_end3;

      -----------------------------------------------------------  

      when signore =>
        pc_inc <= '1';
        nstate <= sfetch;
      
      -----------------------------------------------------------
      
      when shalt => 
        DONE <= '1';
        nstate <= shalt;
    
      when others => null;
    end case;
  end process;
end behavioral;

