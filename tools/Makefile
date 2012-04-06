GCC = arm-none-eabi-gcc
GPP = nspire-bflt-g++
GPPEXISTS := $(shell nspire-bflt-g++ --version > /dev/null 2>&1; echo $$?)
GCCFLAGS = -fpic -Wall -W -mcpu=arm926ej-s
OBJS = crt0.o
CPPOBJS = c++_core.o

ifeq ("${GPPEXISTS}","0")
all: $(OBJS) script c++_support
	# C++ support compiled
else
all: $(OBJS) script
	# C++ support not compiled
endif

%.o: %.S
	$(GCC) $(GCCFLAGS) -c $<

%.o: %.cpp
	$(GPP) $(GCCFLAGS) -c $<

script: nspire-bflt-gcc nspire-bflt-g++ nspire-bflt-ld
	chmod +x $^

c++_support: script $(CPPOBJS)

clean:
	rm -f *.o
