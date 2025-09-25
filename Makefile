.PHONY: clean gamecube wii

VERSION	:=	v4.2.0

all: gamecube wii

gamecube:
	@echo Building GameCube version
	@$(MAKE) --no-print-directory -f $(abspath $(dir $(lastword $(MAKEFILE_LIST))))/common.mk GC=1 VERSION=$(VERSION)

wii:
	@echo Building Wii version
	@$(MAKE) --no-print-directory -f $(abspath $(dir $(lastword $(MAKEFILE_LIST))))/common.mk VERSION=$(VERSION)

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
