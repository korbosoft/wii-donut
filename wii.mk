include $(DEVKITPPC)/wii_rules

BUILD		:=	build-wii

CFLAGS		=	-g -O2 -fexec-charset=CP437 -Wall -DVERSION=\"$(VERSION)\" $(MACHDEP) $(INCLUDE)
CXXFLAGS	=	$(CFLAGS)

LIBS		:=	-lfat -lwiiuse -lbte -lgrrmod -laesnd -logc -lm

export OUTPUT	:=	$(abspath $(dir $(firstword $(MAKEFILE_LIST))))/boot
