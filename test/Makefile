GCC = nspire-bflt-gcc
LD = nspire-bflt-ld
GCCFLAGS = -Wall -W
LDFLAGS =
EXE = test.bflt.tns
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
vpath %.tns $(DISTDIR)

all: $(EXE)

%.o: %.c
$(GCC) $(GCCFLAGS) -c $<
%.o: %.S
$(GCC) $(GCCFLAGS) -c $<

$(EXE): $(OBJS)
$(LD) $^ -o $@ $(LDFLAGS)

clean:
rm -f *.o *.gdb a.out
rm -f $(EXE)