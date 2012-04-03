GCC = nspire-bflt-gcc
LD = nspire-bflt-ld
AR = arm-none-eabi-ar
NM = arm-none-eabi-nm
OBJCOPY = arm-none-eabi-objcopy
GCCFLAGS = -Wall -W
LDFLAGS =
EXE = test.bflt.tns
LIBID = 3
LIB = lib$(LIBID).so.tns
LIBOBJS = lib.o
EXEOBJS = main.o
EXPORTLIST = exports.sym
EXPORTSYMBOLS = `awk '{printf "-G %s ", $$0}' "$(EXPORTLIST)"`

vpath %.tns $(DISTDIR)

all: $(LIB) $(EXE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -DLIB_ID=$(LIBID) -c $<

$(EXE): $(EXEOBJS)
	$(LD) -Wl,-dy -Wl,-R,$(LIB).gdb $^ -o $@ $(LDFLAGS)

$(LIB): $(LIBOBJS) $(EXPORTLIST)
	$(AR) -r $(LIB:.so.tns=.a) $(LIBOBJS)
	$(LD) $(LDFLAGS) -mid-shared-library -Wl,-shared-lib-id,$(LIBID) -Wl,--whole-archive,$(LIB:.so.tns=.a),--no-whole-archive -o $@
	$(LD) $(LDFLAGS) -mid-shared-library -Wl,-shared-lib-id,0 -Wl,--whole-archive,$(LIB:.so.tns=.a),--no-whole-archive -o lib.so.tns
	mv lib.so.tns $(LIB)
	$(OBJCOPY) $(EXPORTSYMBOLS) $(LIB).gdb
	@echo =======
	@echo The following symbols will be exported
	@echo
	@$(NM) lib3.so.tns.gdb -g | sort
	@echo =======

clean:
	rm -f *.o *.gdb a.out *.tns *.a