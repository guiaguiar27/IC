CONTIKI_PROJECT = node
all: $(CONTIKI_PROJECT)

PLATFORMS_ONLY = z1

CONTIKI=../../..

# force Security from command line
MAKE_WITH_SECURITY ?= 0

MAKE_MAC = MAKE_MAC_TSCH
#A NET layer that does nothing. Packets are relayed up/down the stack, unmodified.
MAKE_NET = MAKE_NET_NULLNET

include $(CONTIKI)/Makefile.dir-variables
MODULES += $(CONTIKI_NG_SERVICES_DIR)/shell

ifeq ($(MAKE_WITH_SECURITY),1)
CFLAGS += -DWITH_SECURITY=1
endif

include $(CONTIKI)/Makefile.include