ifndef Ps5PayloadSdk
Ps5PayloadSdk := $(CURDIR)/../ps5-payload-sdk/install
endif

include $(Ps5PayloadSdk)/toolchain/prospero.mk

Target  := power_control_ps5.bin
Cc      := x86_64-prospero-elf-gcc
Objcopy := x86_64-prospero-elf-objcopy
Cflags  := -O2 -Wall -std=gnu11
Ldflags := -T $(Ps5PayloadSdk)/toolchain/link.x -nostdlib

SRCS := main.c
OBJS := $(SRCS:.c=.o)

all: $(Target)

$(Target): power_control_ps5.elf
	$(Objcopy) -O binary power_control_ps5.elf $(Target)

power_control_ps5.elf: $(OBJS)
	$(Cc) $(Cflags) $(Ldflags) -o $@ $(OBJS) -L$(Ps5PayloadSdk)/lib -lkernel -lc

%.o: %.c
	$(Cc) $(Cflags) -I$(Ps5PayloadSdk)/include -c $< -o $@

clean:
	rm -f $(OBJS) power_control_ps5.elf $(Target)

