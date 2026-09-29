MCU_DIR = ../../../../../mcu

-include $(TOP)/.config
ifeq ($(CONFIG_ELOG),y)
CFLAGS += -DHAVE_ELOG
endif


CFLAGS += -I. -I$(TOP)/shared -I$(SRCBASE)/include
CFLAGS += -I$(TOP)/udhcpd
CFLAGS += -I$(TOP)/feixun/wl_client_rate
CFLAGS += -I$(TOP)/libphicomm
CFLAGS += -I$(TOP)/feixun/weather
CFLAGS += -I$(SRCBASE)/router/libbcm
CFLAGS += -I$(SRCBASE)/rc
CFLAGS += -I$(SRCBASE)/common/include
CFLAGS += -I$(SRCBASE)/shared/bcmwifi/include/
CFLAGS += -I$(SRCBASE_ROUTER)/libiconv-1.14/include
CFLAGS += -I$(SRCBASE)/../components/opensource/jsonc/installbin/include/json-c
CFLAGS += -s -O2
LDFLAGS += -L$(TOP)/libbcm -L$(PLATFORMDIR)/install/libbcm/usr/lib -lbcm
LDFLAGS += -L$(TOP)/nvram -L$(INSTALLDIR)/nvram/usr/lib -lnvram
LDFLAGS += -L$(TOP)/libbcmcrypto -L$(INSTALLDIR)/libbcmcrypto/usr/lib -lbcmcrypto
LDFLAGS += -L$(TOP)/shared -L$(INSTALLDIR)/shared/usr/lib -lshared
LDFLAGS += -L$(TOP)/libphicomm -L$(INSTALLDIR)/libphicomm/usr/lib -lwlcr
LDFLAGS += -L$(SRCBASE)/router/libiconv-1.14/lib/.libs/ -liconv
LDFLAGS += -L$(TOP)/../../components/opensource/jsonc/installbin/lib
LDFLAGS += -L$(INSTALLDIR)/libjsonc/usr/lib -ljson-c
LDFLAGS += -L$(SRCBASE)/rc
LDFLAGS += $(EXTRA_LDFLAGS)



UHMI_BIN=uhmi
UHMIMSG_BIN=uhmiMsg
UHMI_TEST=uhmi_test

UHMI_OBJS=uhmi_main.o uhmi_uart.o uhmi_bootloader.o uhmi_application.o uhmi_common.o \
uhmi_data_crawler.o hz2py.o oui.o weather.o
UHMIMSG_OBJS=uhmi_msg.o
UHMITEST_OBJS = test_main.o oui.o

all: $(UHMI_BIN) $(UHMIMSG_BIN) $(UHMI_TEST)
.PHONY:all

$(UHMI_BIN): $(UHMI_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(UHMI_BIN) $(UHMI_OBJS) -lpthread

$(UHMIMSG_BIN):$(UHMIMSG_OBJS)
	 $(CC) $(CFLAGS) -o $(UHMIMSG_BIN) $(UHMIMSG_OBJS)

$(UHMI_TEST): $(UHMITEST_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
	chmod +x $@

install:
	echo "$(SDIR)"
	echo "$(MCU_DIR)"
	install -d $(INSTALLDIR)/usr/sbin
	install -m 755 $(UHMI_BIN) $(INSTALLDIR)/usr/sbin
	install -m 755 $(UHMIMSG_BIN) $(INSTALLDIR)/usr/sbin
	install -d $(TARGETDIR)/etc/oui
	sh ouitxt_to_vendorjs.sh > $(TARGETDIR)/etc/oui/vendor.json
	rm -rf $(TARGETDIR)/mcu
	cp -rf $(MCU_DIR)/app_hex/ $(TARGETDIR)/mcu

.PHONY:	clean
clean:
	rm -f *.o .*.depend $(UHMI_BIN) $(UHMIMSG_BIN) $(UHMI_TEST)

oui.o: $(TOP)/shared/oui.c
	$(CC) -c -o $@ $< $(CFLAGS)

weather.o: $(TOP)/feixun/weather/weather.c
	$(CC) -c -o $@ $< $(CFLAGS)
