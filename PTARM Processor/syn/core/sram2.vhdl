library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

-- SPM will be hooked up to both instruction and data scratch pad ports, this
-- is because we currently don't have a memory management unit to
-- govern which addresses are located on the spms if they are separate, so to
-- be able to write instructions in to execute, we have one SPM currently.
--
-- Write enable is only needed for the second port which is hooked up to the
-- memory stage in the pipeline. The instruction fetch stage doesn't need to
-- write back to the SPM.
--
-- We assume the instruction access (port A) is word aligned, and it has no
-- write port because the pipeline doesn't write back to memory through the
-- instruction access ports
--
-- We attempt to support byte store or half word store
-- CTRLB signal: 0 - word store
--               1 - Byte store

entity SPM is
	generic(
		POWERSIZE : integer := 14;
		DATAWIDTH : integer := 32;
		THREADBITS : integer := 2
	);
	
	port (
		CLK : in std_logic;
		EN_A : in std_logic;															-- Output enable A
		EN_B : in std_logic;                      							-- Output enable B
		ADDR_A : in std_logic_vector(POWERSIZE+THREADBITS-1 downto 0);	-- Address
		DO_A : out std_logic_vector(DATAWIDTH-1 downto 0);					-- Data out
		ADDR_B : in std_logic_vector(POWERSIZE+THREADBITS-1 downto 0);	-- Address
		DI_B : in std_logic_vector(DATAWIDTH-1 downto 0);					-- Data in
		DO_B : out std_logic_vector(DATAWIDTH-1 downto 0);					-- Data out
		WEN_B : in std_logic;														-- Write enable for B
		CTRL : in std_logic															-- Used to control B store type
	); 
end SPM;

architecture sim of SPM is 
	-- NOTE: Assume accesses are word aligned
	subtype word_type is std_logic_vector((DATAWIDTH/4)-1 downto 0);
	type ram_type is array(0 to (2**(POWERSIZE+THREADBITS-2) - 1)) of word_type;	-- Ram will be split into four 1-byte addressable regions

	shared variable RAM0 : ram_type := (others => (others => '0')); 
	shared variable RAM1 : ram_type := (others => (others => '0')); 
	shared variable RAM2 : ram_type := (others => (others => '0')); 
	shared variable RAM3 : ram_type := (others => (others => '0')); 
begin
	-- purpose: Controlling of port A, no write port
	-- type   : sequential
	-- inputs : CLK, EN_A
	-- outputs: DO_A	
	process(CLK)
	variable vaddr_a: integer;
	begin
		if(rising_edge(CLK)) then 
			if(EN_A = '1') then
				vaddr_a := conv_integer(ADDR_A(POWERSIZE+THREADBITS-1 downto 2));
				DO_A <= RAM3(vaddr_a) & RAM2(vaddr_a) & RAM1(vaddr_a) & RAM0(vaddr_a);
			end if;
		end if;
	end process;
	
	-- purpose: Controlling of port B
	-- type   : sequential
	-- inputs : CLK, EN_B
	-- outputs: DO_B
	process(CLK)
	variable vaddr_b: integer;
	begin
		if(rising_edge(CLK)) then
			if(EN_B = '1') then
				vaddr_b := conv_integer(ADDR_B(POWERSIZE+THREADBITS-1 downto 2));
				
				if WEN_B = '1' then
					if CTRL = '0' or ADDR_B(1 downto 0) = "00" then
						RAM0(vaddr_b) := DI_B(7 downto 0);
					end if;
					
					if CTRL = '0' or ADDR_B(1 downto 0) = "01" then 
						RAM1(vaddr_b) := DI_B(15 downto 8);
					end if;
					
					if CTRL = '0' or ADDR_B(1 downto 0) = "10" then
						RAM2(vaddr_b) := DI_B(23 downto 16);
					end if;
					
					if CTRL = '0' or ADDR_B(1 downto 0) = "11" then 
						RAM3(vaddr_b) := DI_B(31 downto 24);
					end if;
				end if;

				DO_B <= RAM3(vaddr_b) & RAM2(vaddr_b) & RAM1(vaddr_b) & RAM0(vaddr_b);
			end if;	
		end if;
	end process;

end sim;
