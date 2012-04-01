GCC = nspire-gcc
LD = nspire-ld
GCCFLAGS = -elf2flt -Os -nostdlib -Wall -W -marm -mcpu=arm926ej-s
LDFLAGS = -Wl,-elf2flt -nostdlib
EXE = test.tns
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
DISTDIR = .
vpath %.tns $(DISTDIR)

all: $(EXE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

$(EXE): $(OBJS)
	$(LD) $^ -o $(@:.tns=.bflt.tns) $(LDFLAGS)

clean:
	rm -f *.o *.elf
	rm -f $(DISTDIR)/$(EXE)
