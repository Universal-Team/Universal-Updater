#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules


#---------------------------------------------------------------------------------
# Version number
#---------------------------------------------------------------------------------
VERSION_MAJOR := 4
VERSION_MINOR := 0
VERSION_MICRO := 0
CURRENT_VERSION := "$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_MICRO)"

#---------------------------------------------------------------------------------
# External tools
#---------------------------------------------------------------------------------
ifeq ($(OS),Windows_NT)
PYTHON	?= py -3

else
PYTHON	?= python3

endif

#---------------------------------------------------------------------------------
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# MUSIC contains a directory of music and sound effect files
# all directories are relative to this makefile
#---------------------------------------------------------------------------------
BUILD		:=	build
SOURCES		:=	../Universal-Core/source \
					../../source ../../source/actions ../../source/data ../../source/menus ../../source/menus/tabs ../../source/store ../../source/utils \
					../../source/nds ../../source/nds/data ../../source/nds/menus ../../source/nds/utils

INCLUDES	:=	../Universal-Core/include \
					../../include ../../include/actions ../../include/data ../../include/menus ../../include/menus/tabs ../../include/store ../../include/utils \
					../../include/nds ../../include/nds/data ../../include/nds/menus ../../include/nds/utils
GRAPHICS	:=	../graphics
MUSIC		:=

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-mthumb-interwork

CFLAGS	:=	-g -Wall -Wno-psabi -O2 \
 			-march=armv5te -mtune=arm946e-s -fomit-frame-pointer \
			-DC_V=\"$(CURRENT_VERSION)\" \
			-ffast-math \
			$(ARCH)

CFLAGS	+=	$(INCLUDE) -DARM9 -D_GNU_SOURCE=1
CXXFLAGS	:=	$(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++20 -Wno-volatile

ASFLAGS	:=	-g $(ARCH) -march=armv5te -mtune=arm946e-s

LDFLAGS	=	-specs=ds_arm9.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:=	-lcurl -lmbedtls -lmbedx509 -lmbedcrypto -larchive -lbz2 -llzma -lm -lz -lfat -ldsiwifi9 -lnds9

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=	../../libs $(PORTLIBS) $(LIBNDS)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export ARM9ELF	:=	$(CURDIR)/$(TARGET).elf
export DEPSDIR := $(CURDIR)/$(BUILD)

export AUDIOFILES	:=	$(foreach dir,$(notdir $(wildcard $(MUSIC)/*.*)),$(CURDIR)/$(MUSIC)/$(dir))

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir))

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*))) # soundbank.bin
TDSFILES	:=	$(foreach dir,$(GRAPHICS),$(notdir $(wildcard $(dir)/*.tds)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export TDXFILES	:=	$(addprefix $(NITRO_FILES)/graphics/,$(TDSFILES:.tds=.tdx))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) *.elf *.nds* *.bin $(NITRO_FILES)/graphics/*.tdx

#---------------------------------------------------------------------------------
else

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(ARM9ELF)	:	$(TDXFILES) $(OFILES)
	@echo linking $(notdir $@)
	@$(LD)  $(LDFLAGS) $(OFILES) $(LIBPATHS) $(LIBS) -o $@

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
$(NITRO_FILES)/graphics/%.tdx: $(CURDIR)/../$(GRAPHICS)/%.tds
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(PYTHON) ../$(GRAPHICS)/img2tdx.py $< -o $@ --header ../$(BUILD)/$(notdir ${@:.tdx=.h})

-include $(DEPSDIR)/*.d

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------