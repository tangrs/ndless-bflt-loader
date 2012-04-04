GCC = nspire-bflt-gcc
LD = nspire-bflt-ld
AR = arm-none-eabi-ar
NM = arm-none-eabi-nm
OBJCOPY = arm-none-eabi-objcopy

GCCFLAGS = -Wall -W
LDFLAGS =

EXE = test.bflt.tns
# Select an ID for your library here
LIBID = 3
LIBOBJS = lib.o
EXEOBJS = main.o

# Everything below here probably doesn't need to be touched
LIB = lib$(LIBID).so.tns
LIBBASE := $(shell printf "%\#.2x000000" $(LIBID))

EXPORTLIST = exports.sym
EXPORTSYMBOLS := $(shell awk '{printf "-G %s ", $$0}' "$(EXPORTLIST)")
WRAPSYMBOLS := $(shell awk '{printf "-Wl,--wrap,%s ", $$0}' "$(EXPORTLIST)")

all: $(LIB) $(EXE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -DLIB_ID=$(LIBID) -c $<

$(EXE): $(EXEOBJS) lib$(LIBID)_wraps.o
	# Link our executable - remembering to attach the wrappers
	$(LD) $(WRAPSYMBOLS) -Wl,-R,$(LIB).gdb $^ -o $@ $(LDFLAGS)

$(LIB): $(LIBOBJS) $(EXPORTLIST) lib$(LIBID)_wraps.o
	$(AR) -r $(LIB:.so.tns=.a) $(LIBOBJS)

	# We use an interesting hack here to stop a huge file from being produced
	# Since we're linking at $(LIBBASE) as the base, everything between 0x00000000 and $(LIBBASE)
	# will be useless padding that can take up to 4064MB (yes, that's right).

	# First we link the library at base 0x00000000 to produce our library (no padding will be generated)
	# This will make $(LIB) (our actual library that we send to the calculator)
	# and $(LIB).gdb (the file containing the addresses of symbols)
	$(LD) $(LDFLAGS) -mid-shared-library -Wl,-shared-lib-id,0 -Wl,--whole-archive,$(LIB:.so.tns=.a),--no-whole-archive -o $@

	# Then we change the base addresses in $(LIB).gdb to the proper library base address $(LIBBASE)
	# This is okay because our code is PIC anyway
	$(OBJCOPY) --change-addresses $(LIBBASE) $(LIB).gdb

	# Whitelist symbols to export
	$(OBJCOPY) $(EXPORTSYMBOLS) $(LIB).gdb
	@echo =======
	@echo The following symbols will be exported
	@echo
	@$(NM) $(LIB).gdb -g | sort
	@echo =======

lib$(LIBID)_wraps.S:
	# Generate wrappers to wrap shared library calls so they don't crash due to relocation problems
	# They are basically custom code veneers since the automatically generated ones suck and
	# don't get relocated properly

	# These wrapper object files must be linked to all executabled using this shared library

	echo .section .shlb >> lib$(LIBID)_wraps.S
	awk '{printf "__wrap_%s_addr: .word __real_%s\n", $$0, $$0}' "$(EXPORTLIST)" >> lib$(LIBID)_wraps.S
	awk '{printf "__wrap_%s: .global __wrap_%s\n    ldr pc,__wrap_%s_addr\n\n", $$0, $$0, $$0}' "$(EXPORTLIST)" >> lib$(LIBID)_wraps.S

lib$(LIBID)_wraps.o: lib$(LIBID)_wraps.S
	$(GCC) $(GCCFLAGS) -c $<

clean:
	rm -f *.o *.gdb a.out *.tns *.a lib$(LIBID)_wraps.S