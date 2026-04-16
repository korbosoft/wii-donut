.PHONY: clean gba gamecube wii

VERSION	:=	v5.1.1

CURDIR = $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

all: gba gamecube wii

gba:
	@echo Building GBA version
	@$(MAKE) --no-print-directory -C $(CURDIR)/gba

gamecube:
	@echo Building GameCube version
	@$(MAKE) --no-print-directory -f $(CURDIR)/common.mk GC=1 VERSION=$(VERSION)

wii:
	@echo Building Wii version
	@$(MAKE) --no-print-directory -f $(CURDIR)/common.mk VERSION=$(VERSION)

clean:
	@echo clean ...
	@rm -fr build-* *.elf *.dol

release:
	@mkdir -p korbodonut
	@cp boot.dol meta.xml icon.png ./korbodonut/
	@cp donut-gc.elf donut-gc-debug.elf
	@cp boot.elf donut-wii-debug.elf
	@zip -rv donut ./korbodonut/
	@gh release create $(VERSION) donut.zip donut-gc.dol donut-wii-debug.elf donut-gc-debug.elf
	@rm -fr ./korbodonut/ donut-wii-debug.elf donut-gc-debug.elf donut.zip

