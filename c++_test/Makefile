GCC = nspire-bflt-gcc
GPP = nspire-bflt-g++
LD = nspire-bflt-ld
GCCFLAGS = -Wall -W
LDFLAGS =
EXE = test.bflt.tns
OBJS = main.o
vpath %.tns $(DISTDIR)

all: $(EXE)

%.o: %.cpp
	$(GPP) $(GCCFLAGS) -c $<
%.o: %.S
	$(GCC) $(GCCFLAGS) -c $<

$(EXE): $(OBJS)
	$(LD) $^ -o $@ $(LDFLAGS)

clean:
	rm -f *.o *.gdb a.out
	rm -f $(EXE)