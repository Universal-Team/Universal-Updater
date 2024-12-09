#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

ifneq (,$(shell which python3))
PYTHON	:= python3
else ifneq (,$(shell which python2))
PYTHON	:= python2
else ifneq (,$(shell which python))
PYTHON	:= python
else
$(error "Python not found in PATH, please install it.")
endif

# These set the information text in the nds file
GAME_TITLE    := Universal-Updater
GAME_SUBTITLE :=
GAME_AUTHOR   := Universal-Team
GAME_CODE     := KUUA

include $(DEVKITARM)/ds_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# DATA is a list of directories containing binary files embedded using bin2o
# GRAPHICS is a list of directories containing image files to be converted with grit
# AUDIO is a list of directories containing audio to be converted by maxmod
# ICON is the image used to create the game icon, leave blank to use default rule
# NITRO is a directory that will be accessible via NitroFS
#---------------------------------------------------------------------------------
TARGET   := Universal-Updater
BUILD    := build
SOURCES  := ../source ../source/actions ../source/data ../source/menus \
            ../source/menus/tabs ../source/store ../source/utils \
            ../source/nds ../source/nds/data ../source/nds/utils \
            Universal-Core/source
INCLUDES := ../include ../include/actions ../include/data ../include/menus \
            ../include/menus/tabs ../include/store ../include/utils \
            ../include/nds ../include/nds/data ../include/nds/utils \
            Universal-Core/include
DATA     :=
GRAPHICS := graphics
AUDIO    :=
ICON     := icon.bmp

# specify a directory which contains the nitro filesystem
# this is relative to the Makefile
NITRO    := nitrofiles

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH := -march=armv5te -mtune=arm946e-s

CFLAGS   := -g -Wall -O2 -ffunction-sections -fdata-sections\
            $(ARCH) $(INCLUDE) -DARM9
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++20
ASFLAGS  := -g $(ARCH)
LDFLAGS   = -specs=ds_arm9.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project (order is important)
#---------------------------------------------------------------------------------
LIBS := -lcurl -lmbedtls -lmbedx509 -lmbedcrypto -larchive -lbz2 -llzma -lm -lz -lfat -ldswifi9 -lnds9

# automagically add maxmod library
ifneq ($(strip $(AUDIO)),)
LIBS := -lmm9 $(LIBS)
endif

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS := $(CURDIR)/libs $(LIBNDS) $(PORTLIBS)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT := $(CURDIR)/$(TARGET)

export VPATH := $(CURDIR)/$(subst /,,$(dir $(ICON)))\
                $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))\
                $(foreach dir,$(DATA),$(CURDIR)/$(dir))\
                $(foreach dir,$(GRAPHICS),$(CURDIR)/$(dir))

export DEPSDIR := $(CURDIR)/$(BUILD)

CFILES   := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES   := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
PNGFILES := $(foreach dir,$(GRAPHICS),$(notdir $(wildcard $(dir)/*.png)))
TDSFILES := $(foreach dir,$(GRAPHICS),$(notdir $(wildcard $(dir)/*.tds)))
BINFILES := $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*))) $(PNGFILES:.png=.grf)

# prepare NitroFS directory
ifneq ($(strip $(NITRO)),)
  export NITRO_FILES := $(CURDIR)/$(NITRO)
endif

# get audio list for maxmod
ifneq ($(strip $(AUDIO)),)
  export MODFILES	:=	$(foreach dir,$(notdir $(wildcard $(AUDIO)/*.*)),$(CURDIR)/$(AUDIO)/$(dir))

  # place the soundbank file in NitroFS if using it
  ifneq ($(strip $(NITRO)),)
    export SOUNDBANK := $(NITRO_FILES)/soundbank.bin

  # otherwise, needs to be loaded from memory
  else
    export SOUNDBANK := soundbank.bin
    BINFILES += $(SOUNDBANK)
  endif
endif

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
  export LD := $(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
  export LD := $(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_BIN := $(addsuffix .o,$(BINFILES))

export OFILES_SOURCES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export OFILES := $(OFILES_BIN) $(OFILES_SOURCES)

export HFILES := $(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE := $(foreach dir,$(INCLUDES),-iquote $(CURDIR)/$(dir))\
                  $(foreach dir,$(LIBDIRS),-I$(dir)/include)\
                 -I$(CURDIR)/$(BUILD)
export LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export TDXFILES := $(addprefix $(NITRO_FILES)/graphics/,$(TDSFILES:.tds=.tdx))

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.bmp)

	ifneq (,$(findstring $(TARGET).bmp,$(icons)))
		export GAME_ICON := $(CURDIR)/$(TARGET).bmp
	else
		ifneq (,$(findstring icon.bmp,$(icons)))
			export GAME_ICON := $(CURDIR)/icon.bmp
		endif
	endif
else
	export GAME_ICON := $(CURDIR)/$(ICON)
endif

ifeq ($(strip $(GAME_SUBTITLE)),)
	export GAME_FULL_TITLE := $(GAME_TITLE);$(GAME_AUTHOR)
else
	export GAME_FULL_TITLE := $(GAME_TITLE);$(GAME_SUBTITLE);$(GAME_AUTHOR)
endif

.PHONY: $(BUILD) all clean

#---------------------------------------------------------------------------------
$(BUILD):
	@mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).nds $(TARGET).dsi $(SOUNDBANK) $(TDXFILES)

#---------------------------------------------------------------------------------
else

#---------------------------------------------------------------------------------
# Get version number from git
#---------------------------------------------------------------------------------
ifneq ($(shell echo $(shell git tag -l --points-at HEAD) | head -c 1),) # If on a tagged commit, use just tag
GIT_VER := $(shell git tag -l --points-at HEAD)
else # Otherwise include commit
GIT_VER := $(shell git describe --abbrev=0 --tags)-$(shell git rev-parse --short=7 HEAD)
endif

# Print new version if changed
ifeq (,$(findstring $(GIT_VER), $(shell cat version.hpp)))
$(shell printf "#ifndef VERSION_HPP\n#define VERSION_HPP\n\n#define VER_NUMBER \"$(GIT_VER)\"\n\n#endif\n" > version.hpp)
endif

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: $(OUTPUT).nds $(OUTPUT).dsi

$(OUTPUT).nds: $(OUTPUT).elf $(NITRO_FILES)
	$(SILENTCMD)ndstool -c $@ -9 $(OUTPUT).elf $(_ARM7_ELF) $(_ADDFILES) \
		-b $(GAME_ICON) "$(GAME_FULL_TITLE)" \
		-z 80040000
	@echo built ... $(notdir $@)

$(OUTPUT).dsi: $(OUTPUT).elf $(NITRO_FILES)
	$(SILENTCMD)ndstool -c $@ -9 $(OUTPUT).elf $(_ARM7_ELF) $(_ADDFILES) \
		-b $(GAME_ICON) "$(GAME_FULL_TITLE)" \
		-g $(GAME_CODE) 00 "UPDATER" -z 80040000 -u 00030004
	@echo built ... $(notdir $@)

$(OUTPUT).elf: $(OFILES)

# source files depend on generated headers
$(OFILES_SOURCES) : $(HFILES)

# need to build soundbank and images first
$(OFILES): $(SOUNDBANK) $(TDXFILES)

#---------------------------------------------------------------------------------
# rule to build solution from music files
#---------------------------------------------------------------------------------
$(SOUNDBANK) : $(MODFILES)
#---------------------------------------------------------------------------------
	mmutil $^ -d -o$@ -hsoundbank.h

#---------------------------------------------------------------------------------
%.bin.o %_bin.h : %.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
$(NITRO_FILES)/graphics/%.tdx ../$(BUILD)/%.h : $(CURDIR)/../$(GRAPHICS)/%.tds
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(PYTHON) ../$(GRAPHICS)/img2tdx.py $< -o $@ --header ../$(BUILD)/$(notdir ${@:.tdx=.h})

#---------------------------------------------------------------------------------
# This rule creates grf files using grit
# grit takes an image file and a .grit describing how the file is to be processed
# add additional rules like this for each image extension
# you use in the graphics folders
#---------------------------------------------------------------------------------
%.grf: %.png %.grit
#---------------------------------------------------------------------------------
	grit $< -ftr -fh! -o$*
#---------------------------------------------------------------------------------
%.grf.o %_grf.h: %.grf
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPSDIR)/*.d

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
