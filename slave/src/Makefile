all: build

build: | ../build
	mv -f main.bin main.elf main.hex main.lst main.map ../build/ 2>/dev/null || true

../build:
	mkdir -p ../build/

clean:
	rm -rf ../build/ || true

TARGET := main
ADDITIONAL_C_FILES := kcdk_debug.c kcdk_hash.c kcdk_helpers.c kcdk_packet.c kcdk_usart.c
PREFIX = riscv64-elf

CFLAGS += -O1 -funroll-loops -ffunction-sections -fdata-sections
LDFLAGS += -Wl,--gc-sections

include ../lib/ch32fun/ch32v003fun/ch32v003fun.mk
