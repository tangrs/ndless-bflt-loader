GCC = nspire-gcc
LD = nspire-ld
GCCFLAGS = -elf2flt -Os -nostdlib -Wall -W -marm -mcpu=arm926ej-s
LDFLAGS = -T "ldscript" -Wl,-elf2flt -nostdlib
EXE = test.bflt.tns
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
vpath %.tns $(DISTDIR)

all: $(EXE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

$(EXE): $(OBJS)
	$(LD) $^ -o $@ $(LDFLAGS)

clean:
	rm -f *.o *.gdb a.out
	rm -f $(EXE)
