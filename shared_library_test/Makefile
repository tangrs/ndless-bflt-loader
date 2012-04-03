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

EXPORTLIST = exports.sym
EXPORTSYMBOLS = `awk '{printf "-G %s ", $$0}' "$(EXPORTLIST)"`
WRAPSYMBOLS = `awk '{printf "-Wl,--wrap,%s ", $$0}' "$(EXPORTLIST)"`

all: $(LIB) $(EXE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -DLIB_ID=$(LIBID) -c $<

$(EXE): $(EXEOBJS) _lib$(LIBID)_wraps.o
	# Link our executable - remembering to attach the wrappers
	$(LD) $(WRAPSYMBOLS) -Wl,-R,$(LIB).gdb $^ -o $@ $(LDFLAGS)

$(LIB): $(LIBOBJS) $(EXPORTLIST) _lib$(LIBID)_wraps.o
	$(AR) -r $(LIB:.so.tns=.a) $(LIBOBJS)

	# We use an interesting hack here to stop a huge >16MB file from being produced as the library
	# Most of the huge file is padding anyway (since we're linking at 0x[ID]000000 as the base)

	# First we compile the library for linking with other executables
	# This will give us our $(LIB).gdb file
	$(LD) $(LDFLAGS) -mid-shared-library -Wl,-shared-lib-id,$(LIBID) -Wl,--whole-archive,$(LIB:.so.tns=.a),--no-whole-archive -o $@

	# Then re-link with base of 0x0 for the actual binary that is to be loaded into memory
	# This assumes that the addressing won't change between previous link and this one
	# This will give us our actual $(LIB) file
	# TODO: change into a objcopy call or something
	$(LD) $(LDFLAGS) -mid-shared-library -Wl,-shared-lib-id,0 -Wl,--whole-archive,$(LIB:.so.tns=.a),--no-whole-archive -o lib.so.tns

	# Overwrite the huge bFLT one with the one linked at 0x0
	mv lib.so.tns $(LIB)

	# Be selective about the symbols that are exported
	$(OBJCOPY) $(EXPORTSYMBOLS) $(LIB).gdb
	@echo =======
	@echo The following symbols will be exported
	@echo
	@$(NM) $(LIB).gdb -g | sort
	@echo =======

_lib$(LIBID)_wraps.S:
	# Generate wrappers to wrap shared library calls so they don't crash due to relocation problems
	# They are basically custom code veneers since the automatically generated ones suck and
	# don't get relocated properly

	# These wrapper object files must be linked to all executabled using this shared library

	echo .section .shlb >> _lib$(LIBID)_wraps.S
	awk '{printf "__wrap_%s_addr: .word __real_%s\n", $$0, $$0}' "$(EXPORTLIST)" >> _lib$(LIBID)_wraps.S
	awk '{printf "__wrap_%s: .global __wrap_%s\n    ldr pc,__wrap_%s_addr\n\n", $$0, $$0, $$0}' "$(EXPORTLIST)" >> _lib$(LIBID)_wraps.S

_lib$(LIBID)_wraps.o: _lib$(LIBID)_wraps.S
	$(GCC) $(GCCFLAGS) -c $<

clean:
	rm -f *.o *.gdb a.out *.tns *.a _lib$(LIBID)_wraps.S