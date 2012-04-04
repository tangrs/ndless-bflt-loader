GCC = nspire-bflt-gcc
LD = nspire-bflt-ld
AR = arm-none-eabi-ar
NM = arm-none-eabi-nm
OBJCOPY = arm-none-eabi-objcopy

GCCFLAGS = -Wall -W
LDFLAGS =

EXE := test.bflt.tns
OBJS := main.o
LIBS := lib3.so.tns
LIBPATH := ../example_shared_lib/bin

LIBLINKOBJ := $(EXE:.bflt.tns=.bflt.tns.o)
WRAPFLAGS :=

vpath lib%.so.tns $(LIBPATH)
vpath lib%.so.tns.gdb $(LIBPATH)
vpath lib%_wraps.o $(LIBPATH)

# Everything below here probably doesn't need to be touched

all: wrapper_flags $(EXE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

lib%.so.tns.gdb:
	$(error Missing symbol file $@)

lib%_wraps.o:
	$(error Missing wrap file $@)

# Generate list of exported symbols from shared libraries and convert them to linker flags
wrapper_flags: $(LIBS:.so.tns=.so.tns.gdb)
	$(eval WRAPFLAGS := $(shell $(NM) -g $(^) | awk '/.+ .+ .+/ {printf "-Wl,--wrap,%s ", $$3}' ))

# Group all shared library symbols into one file
$(LIBLINKOBJ): $(LIBS:%.so.tns=%.so.tns.gdb)
	$(GCC) $(^:%=-Wl,-R,%) -r -o $@ $(LDFLAGS)

# Link everything together
$(EXE): $(LIBLINKOBJ) $(OBJS) $(LIBS:.so.tns=_wraps.o)
	$(LD) $^ -o $@ $(WRAPFLAGS) $(LDFLAGS)

clean:
	rm -f *.o $(EXE)