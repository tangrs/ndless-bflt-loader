GCC = arm-none-eabi-gcc
GCCFLAGS = -fpic -Wall -W -mcpu=arm926ej-s
OBJS = crt0.o

all: $(OBJS) script

%.o: %.S
	$(GCC) $(GCCFLAGS) -c $<

script: nspire-bflt-gcc nspire-bflt-ld
	chmod +x nspire-bflt-gcc nspire-bflt-ld

clean:
	rm -f *.o
