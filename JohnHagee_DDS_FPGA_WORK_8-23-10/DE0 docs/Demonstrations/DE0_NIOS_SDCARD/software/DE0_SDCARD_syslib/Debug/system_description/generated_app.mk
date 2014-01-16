# generated_app.mk
#
# Machine generated for a CPU named "cpu" as defined in:
# d:\NIOS_II\DE0\CD_EXAMPLE\DE0_NIOS_SDCARD\software\DE0_SDCARD_syslib\..\..\DE0_SOPC.ptf
#
# Generated: 2009-05-19 20:47:20.328

# DO NOT MODIFY THIS FILE
#
#   Changing this file will have subtle consequences
#   which will almost certainly lead to a nonfunctioning
#   system. If you do modify this file, be aware that your
#   changes will be overwritten and lost when this file
#   is generated again.
#
# DO NOT MODIFY THIS FILE

# assuming the Quartus project directory is the same as the PTF directory
QUARTUS_PROJECT_DIR = D:/NIOS_II/DE0/CD_EXAMPLE/DE0_NIOS_SDCARD

# the simulation directory is a subdirectory of the PTF directory
SIMDIR = $(QUARTUS_PROJECT_DIR)/DE0_SOPC_sim

DBL_QUOTE := "


# macros defined by/for cfi_flash
CFI_FLASH_FLASHTARGET_ALT_SIM_PREFIX = $(CFI_FLASH_FLASHTARGET_TMP1:0=)
CFI_FLASH_FLASHTARGET_TMP1 = $(ALT_SIM_OPTIMIZE:1=RUN_ON_HDL_SIMULATOR_ONLY_)
BOOT_COPIER = boot_loader_cfi.srec
CPU_CLASS = altera_nios2
CPU_RESET_ADDRESS = 0x2808000


all: delete_placeholder_warning hex sim flashfiles

delete_placeholder_warning: do_delete_placeholder_warning
.PHONY: delete_placeholder_warning

hex: $(QUARTUS_PROJECT_DIR)/onchip_mem.hex
.PHONY: hex

sim: $(SIMDIR)/dummy_file
.PHONY: sim

verifysysid: dummy_verifysysid_file
.PHONY: verifysysid

flashfiles: $(CFI_FLASH_FLASHTARGET_ALT_SIM_PREFIX)cfi_flash.flash
.PHONY: flashfiles

do_delete_placeholder_warning:
	rm -f $(SIMDIR)/contents_file_warning.txt
.PHONY: do_delete_placeholder_warning

$(QUARTUS_PROJECT_DIR)/onchip_mem.hex: $(ELF)
	@echo Post-processing to create $(notdir $@)
	elf2hex $(ELF) 0x02808000 0x280F7FF --width=32 $(QUARTUS_PROJECT_DIR)/onchip_mem.hex --create-lanes=0

$(SIMDIR)/dummy_file: $(ELF)
	if [ ! -d $(SIMDIR) ]; then mkdir $(SIMDIR) ; fi
	@echo Hardware simulation is not enabled for the target SOPC Builder system. Skipping creation of hardware simulation model contents and simulation symbol files. \(Note: This does not affect the instruction set simulator.\)
	touch $(SIMDIR)/dummy_file

dummy_verifysysid_file:
	nios2-download $(JTAG_CABLE)                                --sidp=0x000000e0 --id=40899754 --timestamp=1242736836
.PHONY: dummy_verifysysid_file

$(CFI_FLASH_FLASHTARGET_ALT_SIM_PREFIX)cfi_flash.flash: $(ELF)
	@echo Post-processing to create $(notdir $@)
	elf2flash --input=$(ELF) --flash= --boot=$(DBL_QUOTE)$(shell $(DBL_QUOTE)$(QUARTUS_ROOTDIR)/sopc_builder/bin/find_sopc_component_dir$(DBL_QUOTE) $(CPU_CLASS) $(QUARTUS_PROJECT_DIR))/$(BOOT_COPIER)$(DBL_QUOTE) --outfile=$(CFI_FLASH_FLASHTARGET_ALT_SIM_PREFIX)cfi_flash.flash --sim_optimize=$(ALT_SIM_OPTIMIZE) --base=0x2400000 --end=0x27FFFFF --reset=$(CPU_RESET_ADDRESS)


generated_app_clean:
	$(RM) $(QUARTUS_PROJECT_DIR)/onchip_mem.hex
	$(RM) $(SIMDIR)/dummy_file
	$(RM) $(CFI_FLASH_FLASHTARGET_ALT_SIM_PREFIX)cfi_flash.flash
.PHONY: generated_app_clean
