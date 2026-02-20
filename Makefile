.PHONY: clean gba gamecube wii

VERSION	:=	v4.4.0

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
	@rm -fr ./korbodonut/
	@gh release create $(VERSION) donut.zip donut-gc.dol donut-wii-debug.elf donut-gc-debug.elf
	@rm -f donut-wii.map donut-gc.map donut-wii.zip
