This LEON design is tailored to the Digilent CMOD A7 35T board
--------------------------------------------------------------

Simulation and synthesis
------------------------

The design currently supports synthesis with Xilinx Vivado (tested with 2019.1).

Program the FPGA:                      make vivado-prog-fpga
Program the PROM:                      make vivado-prog-flash
Synthesise and p&r with Xilinx Vivado: make vivado

Design specifics
----------------

* In order to connect through the DSU UART interface use the second UART of the FT2232H.

* System reset is mapped to BTN0

* The SRAM is accessed asynchronously and has an access time of 8ns.
  Waitstates can be set to minimum.

* The design uses SPIM, Instruction zero is taken from 0x220000. (shared with bitfile)

* The application UART0 is connected: PI01 RX (input FPGA), PIO2 TX (output FPGA)

* Booting from SPI Flash required a quirky workaround: The reset generator emulates
  bouncy reset switch and resets the system twice. (WTF!)

* Building the PROM also builds the a hello world application and wraps it with
  mkprom2 to be placed in the prom.

* Output from GRMON is:

user@box$ sudo grmon -u -uart /dev/ttyUSB1 -baud 230400


  GRMON debug monitor v3.2.2 64-bit eval version
  
  Copyright (C) 2020 Cobham Gaisler - All rights reserved.
  For latest updates, go to http://www.gaisler.com/
  Comments or bug-reports to support@gaisler.com
  
  This eval version will expire on 28/09/2020

  using port /dev/ttyUSB1 @ 230400 baud
  UART initialization failure, retrying
  GRLIB build version: 4246
  Detected frequency:  75.0 MHz
  
  Component                            Vendor
  LEON3 SPARC V8 Processor             Cobham Gaisler
  LEON3 SPARC V8 Processor             Cobham Gaisler
  AHB Debug UART                       Cobham Gaisler
  AHB/APB Bridge                       Cobham Gaisler
  LEON3 Debug Support Unit             Cobham Gaisler
  Single-port AHB SRAM module          Cobham Gaisler
  LEON2 Memory Controller              European Space Agency
  SPI Memory Controller                Cobham Gaisler
  Generic UART                         Cobham Gaisler
  Multi-processor Interrupt Ctrl.      Cobham Gaisler
  Modular Timer Unit                   Cobham Gaisler
  
  Use command 'info sys' to print a detailed report of attached cores

grmon3> info sys
  cpu0      Cobham Gaisler  LEON3 SPARC V8 Processor    
            AHB Master 0
  cpu1      Cobham Gaisler  LEON3 SPARC V8 Processor    
            AHB Master 1
  ahbuart0  Cobham Gaisler  AHB Debug UART    
            AHB Master 2
            APB: 80000700 - 80000800
            Baudrate 230400, AHB frequency 75.00 MHz
  apbmst0   Cobham Gaisler  AHB/APB Bridge    
            AHB: 80000000 - 80100000
  dsu0      Cobham Gaisler  LEON3 Debug Support Unit    
            AHB: 90000000 - a0000000
            AHB trace: 128 lines, 32-bit bus
            CPU0:  win 8, itrace 128, V8 mul/div, lddel 1
                   stack pointer 0x4007fff0
                   icache 2 * 8 kB, 16 B/line
                   dcache 2 * 4 kB, 16 B/line
            CPU1:  win 8, itrace 128, V8 mul/div, lddel 1
                   stack pointer 0x4007fff0
                   icache 2 * 8 kB, 16 B/line
                   dcache 2 * 4 kB, 16 B/line
  ahbram0   Cobham Gaisler  Single-port AHB SRAM module    
            AHB: a0000000 - a0100000
            32-bit SRAM: 4 kB @ 0xa0000000
  mctrl0    European Space Agency  LEON2 Memory Controller    
            AHB: 40000000 - 80000000
            APB: 80000000 - 80000100
            8-bit static ram: 1 * 512 kbyte @ 0x40000000
  spim0     Cobham Gaisler  SPI Memory Controller    
            AHB: fff70000 - fff70100
            AHB: 00000000 - 00400000
            IRQ: 7
            SPI memory device read command: 0x0b
  uart0     Cobham Gaisler  Generic UART    
            APB: 80000100 - 80000200
            IRQ: 2
            Baudrate 38422, FIFO debug mode available
  irqmp0    Cobham Gaisler  Multi-processor Interrupt Ctrl.    
            APB: 80000200 - 80000300
  gptimer0  Cobham Gaisler  Modular Timer Unit    
            APB: 80000300 - 80000400
            IRQ: 8
            8-bit scalar, 2 * 32-bit timers, divisor 75

grmon3> load hello.elf
          40000000 .text             26.1kB /  26.1kB   [===============>] 100%
          40006850 .rodata            128B              [===============>] 100%
          400068D0 .data              1.2kB /   1.2kB   [===============>] 100%
  Total size: 27.36kB (129.48kbit/s)
  Entry point 0x40000000
  Image ./unit-test/hello.elf loaded
  
grmon3> run
Hello World

  CPU 0:  Program exited normally
  CPU 1:  Power down mode

grmon3> exit

Exiting GRMON

IP Core Instantiation
----------------------

Bus structure
MST

|  hindex  |  Entity   | IP Core | AHBMI | AHBMO |
| ======== | ========= | ======= | ===== | ===== |
|     0    |   cpu0    | leon3s  |   X   |   0   |
|     1    |   cpu1    | leon3s  |   X   |   1   |
|     2    |   dcom0   | ahbuart |   X   |   2   |
|     3    | ahbjtag0  | ahbjtag |   X   |   3   |
|     5    |  spw0     | spwamba |   X   |   5  |

SLV

|  hindex  |  IRQ  |  Entity   | IP Core | AHBSI | AHBSO |
| ======== | ===== | ========= | ======= | ===== | ===== |
|     1    |       |   apb0    | apbctrl |   X   |   1   |
|     2    |   0   |   dsu0    | dsu3    |   X   |   2   |
|     3    |       |  ahbram0  | ahbram  |   X   |   3   |
|     5    |       |   mctrl0  | mctrl   |   X   |   5   |
|     6    |       |  ahbrom0  | ahbrom  |   X   |   6   |
|     7    |   7   | spimctrl1 | spimctrl|   X   |   7   |


APB

SLV
|  pindex  | paddr  |  irq   |  Entity    | IP Core | APBI | APBO |
| ======== | ====== | =====  | =========  | ======= | ==== | ==== |
|     0    |    0   |        |   mctrl0   | mctrl   |  X   |   0  |
|     1    |    1   |    2   |   uart1    | apbuart |  X   |   1  |
|     2    |    2   |        |   irqctrl0 | irqmp   |  X   |   2  |
|     3    |    3   |    8   |   timer0   | gptimer |  X   |   3  |
|     4    |    7   |        |   dcom0    | ahbuart |  X   |   4  |
|     5    |    5   |    5   |   spw0     | spwamba |  X   |   5  |


IRQ
==

Entity - interconnection
==

|  Entity   | IP Core |  hindex  |  pindex  | AHBMI | AHBMO | AHBSI | AHBSO | APBI | APBO | IRQI | IRQO |
| ========= | ======= | ======== | ======== | ===== | ===== | ===== | ===== | ==== | ==== | ==== | ==== |
|   cpu0    | leon3s  |     0    |    NC    |   X   |   0   |   X   |   X   |  NC  |  NC  |   0  |   0  |
|   cpu1    | leon3s  |     1    |    NC    |   X   |   1   |   X   |   X   |  NC  |  NC  |   1  |   1  |
|   dsu0    | dsu3    |     2    |    NC    |   X   |   NC  |   X   |   2   |  NC  |  NC  |   1  |   1  |
|   dcom0   | ahbuart |     3    |    4     |   X   |   3   |   NC  |   NC  |  NC  |  4   |   NC |   NC |
| ahbjtag0  | ahbjtag |     4    |    NC    |   X   |   4   |   NC  |   NC  |  NC  |  NC  |   NC |   NC |
|   mctrl0  | mctrl   |     5    |    0     |   NC  |   NC  |   X   |   5   |  X   |  0   |   NC |   NC |
|  ahbrom0  | ahbrom  |     6    |    NC    |   NC  |   NC  |   X   |   6   |  NC  |  NC  |   NC |   NC |
| spimctrl1 | spimctrl|     7(7) |    0     |   NC  |   NC  |   X   |   7   |  X   |  0   |   7  |   7  |
|  ahbram0  | ahbram  |     8    |    NC    |   NC  |   NC  |   X   |   8   |  NC  |  NC  |   NC |   NC |
|  apb0     | apbctrl |     9    |    NC    |   NC  |   NC  |   X   |   9   |  NC  |  NC  |   NC |   NC |
|  spw0     | spwamba |     10   |    5(5)  |   X   |   10  |   NC  |   NC  |  X   |   5  |   NC |   NC |
|  ------   | ------  |  ------  | ------   |   --  |   --  |   --  |   --  |  --  |  --  |   -- |   -- |
|  uart1    | apbuart |     NC   |    1(2)  |   NC  |   NC  |   NC  |   NC  |  X   |   1  |   NC |   NC |
|  irqctrl0 | irqmp   |     NC   |    2     |   NC  |   NC  |   NC  |   NC  |  X   |   2  |   X  |   X  |
|  timer0   | gptimer |     NC   |    3(8)  |   NC  |   NC  |   NC  |   NC  |  X   |   3  |   NC |   NC |

