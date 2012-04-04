GCC = nspire-gcc
LD = nspire-ld
GCCFLAGS = -Os -nostdlib -Wall -W -marm
LDFLAGS = -nostdlib
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
EXE = bflt-loader.tns
OBJS = load.o bflt/bflt.o config.o
DISTDIR = .
vpath %.tns $(DISTDIR)

all: $(EXE)

bflt/bflt.o: bflt/bflt.c bflt/bflt_config.h
	$(GCC) $(GCCFLAGS) -c $< -o $@

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

$(EXE): $(OBJS)
	$(LD) $^ -o $(@:.tns=.elf) $(LDFLAGS)
	mkdir -p $(DISTDIR)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $(DISTDIR)/$@

clean:
	rm -f *.o *.elf
	rm -f $(DISTDIR)/$(EXE)
