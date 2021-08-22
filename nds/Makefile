#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

export TARGET	:= Universal-Updater
export TOPDIR	:= $(CURDIR)

# specify a directory which contains the nitro filesystem
# this is relative to the Makefile
NITRO_FILES		:= nitrofiles

# These set the information text in the nds file
GAME_TITLE		:= Universal-Updater
GAME_SUBTITLE1	:= Universal-Team

include $(DEVKITARM)/ds_rules

.PHONY: checkarm9 graphics clean

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: checkarm9 $(TARGET).nds

skip-graphics	:	checkarm9 $(NITRO_FILES) arm9/$(TARGET).elf
	ndstool	-c $(TARGET).nds -9 arm9/$(TARGET).elf \
	-b1 icon.bmp "$(GAME_TITLE);$(GAME_SUBTITLE1)" $(_ADDFILES) \
	-z 80040000 -u 00030004 -a 00000138

#---------------------------------------------------------------------------------
checkarm9:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
$(TARGET).nds	: graphics $(NITRO_FILES) arm9/$(TARGET).elf
	ndstool	-c $(TARGET).nds -9 arm9/$(TARGET).elf \
	-b1 icon.bmp "$(GAME_TITLE);$(GAME_SUBTITLE1)" $(_ADDFILES) \
	-z 80040000 -u 00030004 -a 00000138

#---------------------------------------------------------------------------------
arm9/$(TARGET).elf:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
graphics:
	$(MAKE) -C graphics

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C graphics clean
	rm -f $(TARGET).nds $(TARGET).arm9