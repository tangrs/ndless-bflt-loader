GCC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
GCCFLAGS = -Wl,-elf2flt -fPIC -Os -nostdlib -Wall -W -marm -mcpu=arm926ej-s -I/Users/danieltang/ndless/trunk3.0/include
LDFLAGS = -nostdlib -Wl,-elf2flt
EXE = test.bflt.tns
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
vpath %.tns $(DISTDIR)

all: $(EXE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<
%.o: %.S
	$(GCC) $(GCCFLAGS) -c $<

$(EXE): $(OBJS)
	$(LD) -T "ldscript" $^ -o $@ $(LDFLAGS)

clean:
	rm -f *.o *.gdb a.out
	rm -f $(EXE)
