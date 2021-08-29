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
export NITRO_FILES		:= $(CURDIR)/nitrofiles

# These set the information text in the nds file
GAME_TITLE		:= Universal-Updater
GAME_SUBTITLE1	:= Universal-Team

include $(DEVKITARM)/ds_rules

.PHONY: checkarm7 checkarm9 clean

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: $(TARGET).nds

#---------------------------------------------------------------------------------
$(TARGET).nds	: checkarm7 checkarm9 $(NITRO_FILES)
	ndstool	-c $(TARGET).nds -7 arm7/$(TARGET).elf -9 arm9/$(TARGET).elf \
	-b1 icon.bmp "$(GAME_TITLE);$(GAME_SUBTITLE1)" $(_ADDFILES) \
	-z 80040000 -u 00030004 -a 00000138

#---------------------------------------------------------------------------------
checkarm7:
	$(MAKE) -C arm7

#---------------------------------------------------------------------------------
checkarm9:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	rm -f $(TARGET).nds $(TARGET).arm9