CONTIKI_PROJECT = playstaion
all: $(CONTIKI_PROJECT)
CFLAGS += -DWITH_NODE_ID=1

ifdef NODE_ID
	CFLAGS +=-DNODEID=$(NODE_ID)
endif

TARGET=inga

CONTIKI = ../contiki
include $(CONTIKI)/Makefile.include
