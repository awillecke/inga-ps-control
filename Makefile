# Compile with Movement Control:    make control.upload CONTROL=<BUTTON> MOVEMENT=1 login
# Comile without Movemnet Control:  make control.upload CONTROL=<BUTTON> login
CONTIKI_PROJECT = inga-ps-control
all: $(CONTIKI_PROJECT)
CFLAGS += -DWITH_NODE_ID=1

ifdef CONTROL
    CFLAGS += -DCONTROL=$(CONTROL)
endif

ifdef MOVEMENT
    CFLAGS += -DMOVEMENT=1
endif

ifdef NODE_ID
	CFLAGS +=-DNODEID=$(NODE_ID)
endif

WITH_UIP6=1
UIP_CONF_IPV6=1
CFLAGS += -DUIP_CONF_IPV6_RPL
CFLAGS += -DIPV6=$(IPV6)

TARGET=inga

CONTIKI = ../contiki
include $(CONTIKI)/Makefile.include
