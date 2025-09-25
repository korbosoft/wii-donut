include $(DEVKITPPC)/gamecube_rules

BUILD		:=	build-gc

CFLAGS		=	-g -O2 -fexec-charset=CP437 -Wall -DGC -DVERSION=\"$(VERSION)\" $(MACHDEP) $(INCLUDE)
CXXFLAGS	=	$(CFLAGS)

LIBS		:=	-lfat -lgrrmod -laesnd -logc -lm

export OUTPUT	:=	$(abspath $(dir $(firstword $(MAKEFILE_LIST))))/donut-gc
