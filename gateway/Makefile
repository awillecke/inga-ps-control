CONTIKI_PROJECT = playstation
all: $(CONTIKI_PROJECT)
CFLAGS += -DWITH_NODE_ID=1

ifdef NODE_ID
	CFLAGS +=-DNODEID=$(NODE_ID)
endif

WITH_UIP6=1
UIP_CONF_IPV6=1
CFLAGS += -DUIP_CONF_IPV6_RPL
CFLAGS += -DIPV6=$(IPV6)

TARGET=inga

CONTIKI = ../../../../Contiki/project-cm-2012-inga-contiki
include $(CONTIKI)/Makefile.include
