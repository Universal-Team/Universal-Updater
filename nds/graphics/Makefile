ifeq ($(OS),Windows_NT)
BMP2DS	?= bmp2ds.exe
else
BMP2DS	?= bmp2ds
endif

GRAPHICS	:=	graphics

OUT			:=	../nitrofiles

INPUTS		:=	$(foreach dir,$(GRAPHICS),$(addprefix $(dir)/,$(notdir $(wildcard $(dir)/*.bmp))))

TARGETS		:=	$(INPUTS:.bmp=.gfx)

.PHONY	:	all clean

all	:	$(TARGETS)
	@echo graphics ...

%.gfx	:	%.bmp %.txt
	@mkdir -p $(OUT)/$(@D)
	@$(BMP2DS) $< $(OUT)/$@ -o $(shell cat $(word 2,$^))

%.gfx	:	%.bmp
	@mkdir -p $(OUT)/$(@D)
	@$(BMP2DS) $< $(OUT)/$@

clean	:
	@echo clean ...
	@rm -f bmp2ds
	@rm -rf $(addprefix $(OUT)/,$(TARGETS))