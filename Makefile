SUBDIRS = 3ds nds

all: $(SUBDIRS)

clean:
	@for dir in $(SUBDIRS); do $(MAKE) clean -C $$dir; done

3ds:
	@$(MAKE) -C 3ds
	@mkdir -p bin
	@cp 3ds/Universal-Updater.3dsx bin
	@cp 3ds/Universal-Updater.cia bin

nds:
	@$(MAKE) -C nds
	@mkdir -p bin
	@cp nds/Universal-Updater.nds bin

.PHONY: $(SUBDIRS) all clean
