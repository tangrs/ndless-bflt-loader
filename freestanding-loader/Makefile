GCC = arm-none-eabi-gcc
GCCFLAGS = -Os -nostdlib
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
EXE = loader.bin

all: $(EXE)

$(EXE):
	$(GCC) $(GCCFLAGS) loader.S -o loader.o
	$(OBJCOPY) loader.o -O binary $(EXE)

clean:
	rm -f loader.o $(EXE)
