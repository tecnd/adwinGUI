#!/bin/sh
#
# generated.sh - shell script fragment - not very useful on its own
#
# Machine generated for a CPU named "cpu" as defined in:
# d:\NIOS_II\DE0\CD_EXAMPLE\DE0_NIOS_SDCARD\software\DE0_SDCARD_syslib\..\..\DE0_SOPC.ptf
#
# Generated: 2009-05-19 20:47:22.89

# DO NOT MODIFY THIS FILE
#
#   Changing this file will have subtle consequences
#   which will almost certainly lead to a nonfunctioning
#   system. If you do modify this file, be aware that your
#   changes will be overwritten and lost when this file
#   is generated again.
#
# DO NOT MODIFY THIS FILE

# This variable indicates where the PTF file for this design is located
ptf=d:\NIOS_II\DE0\CD_EXAMPLE\DE0_NIOS_SDCARD\software\DE0_SDCARD_syslib\..\..\DE0_SOPC.ptf

# This variable indicates whether there is a CPU debug core
nios2_debug_core=yes

# This variable indicates how to connect to the CPU debug core
nios2_instance=0

# This variable indicates the CPU module name
nios2_cpu_name=cpu

# These variables indicate what the System ID peripheral should hold
sidp=0x000000e0
id=40899754u
timestamp=1242736836u

# Include operating system specific parameters, if they are supplied.

if test -f /cygdrive/c/altera/90/nios2eds/components/altera_hal/build/os.sh ; then
   . /cygdrive/c/altera/90/nios2eds/components/altera_hal/build/os.sh
fi
