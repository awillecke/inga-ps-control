CONTIKI_PROJECT = inga-ps-control
all: $(CONTIKI_PROJECT)
CFLAGS += -DWITH_NODE_ID=1

ifdef CONTROL
    CFLAGS += -DCONTROL=$(CONTROL)
endif

ifdef NODE_ID
	CFLAGS +=-DNODEID=$(NODE_ID)
endif

TARGET=inga

CONTIKI = ../contiki
include $(CONTIKI)/Makefile.include
