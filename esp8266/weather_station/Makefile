PY = python2
ESPTOOL=$(shell which esptool.py)
CC = xtensa-lx106-elf-gcc
LD = $(CC)
CPPFLAGS = -I.  -mlongcalls -std=c++0x
CFLAGS = -mlongcalls -I. -I./esp8266-dht22 -I/home/rolandas/bin/esp-open-sdk/sdk/include 
LD_SCRIPT = -Teagle.app.v6.ld
SDK_LIBDIR = /home/rolandas/bin/esp-open-sdk/sdk/lib /home/rolandas/bin/esp-open-sdk/sdk/ld

#LDLIBS = -nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -lc -Wl,--end-group -lgcc
LIBS = main net80211 wpa lwip pp phy c gcc hal upgrade 
LDFLAGS		= -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,-static

OBJS = ws.o esp8266-dht22/dht22.o

LIBS := $(addprefix -l,$(LIBS))
SDK_LIBDIR := $(addprefix -L,$(SDK_LIBDIR))


ws-0x00000.bin: ws
	$(PY) $(ESPTOOL) elf2image $^

ws: $(OBJS)
	$(LD) $(SDK_LIBDIR) $(LD_SCRIPT) $(LDFLAGS) -Wl,--start-group $(LIBS) $(OBJS) -Wl,--end-group -o $@


%.o: %.cpp 
	$(CC) -c $< -o $@ $(CPPFLAGS)

%.o: %.c 
	$(CC) -c $< -o $@ $(CFLAGS)

# ws.o: DHT_nonblocking/dht_nonblocking.cpp ws.cpp 

flash: ws-0x00000.bin
	$(PY) $(ESPTOOL) write_flash 0 ws-0x00000.bin 0x10000 ws-0x10000.bin

clean:
	rm -f ws *.o ws-0x00000.bin ws-0x10000.bin esp8266-dht22/*.o
