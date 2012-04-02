GCC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
AR = arm-none-eabi-ar
OBJCOPY = arm-none-eabi-objcopy
GCCFLAGS = -elf2flt -Os -nostdlib -Wall -W -marm -mcpu=arm926ej-s -I/Users/danieltang/ndless/trunk3.0/include
LDFLAGS = -T "ldscript" -Wl,-elf2flt -nostdlib
EXE = test.bflt.tns
LIBID = 3
LIB = lib$(LIBID).so.tns
EXEOBJS = main.o

vpath %.tns $(DISTDIR)

all: $(LIB) $(EXE)

%.o: %.c
	$(GCC) -shared $(GCCFLAGS) -c $<

$(EXE): $(EXEOBJS)
	$(LD) -mshared-library-id=0 -Wl,-R,$(LIB).gdb $^ -o $@ $(LDFLAGS)

$(LIB):
	$(GCC) $(GCCFLAGS) -DLIB_ID=$(LIBID) -c lib.c
	$(GCC) $(GCCFLAGS) -DLIB_ID=$(LIBID) -c dummy.S

	$(AR) -r $(LIB:.lib.bflt.tns=.a) lib.o dummy.o
	$(LD) $(LDFLAGS) -Wl,-shared-lib-id,$(LIBID) dummy.o lib.o -Wl,$(LIB:.lib.bflt.tns=.a) -o $@
	$(OBJCOPY) \
      -L _GLOBAL_OFFSET_TABLE_ \
      -L main \
      -L __main \
      -L __base \
      -L __got_size \
      -L relocate_loop    \
      -L nspire_start \
      -L __saved_syscall_lr        \
      -L _current_shared_library_a5_offset_ \
      $(LIB).gdb

clean:
	rm -f *.o *.gdb a.out *.tns *.a