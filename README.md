# Simple bFLT loader for Ndless

Loads bFLT binaries into memory for Ndless.

## Status

* Can run binaries
* Basic unit tests pass
* No support for gzipped data yet
* GOT relocation untested (is there a good way to test??)
* Shared library support is getting there (but still sketchy)
* Has an SDK!

## API

There's only three functions being exposed:

```int bflt_load(char* filename, void **mem_ptr, size_t* mem_size, int (**entry_address_ptr)(int,char*[]));```

Loads a given filename into memory and correctly relocates it.

The pointer to the final, ready to run binary image will be stored in the pointer pointed to by```mem_ptr```. Will contain NULL on failure.

The size of the image is stored in ```mem_size``` or 0 on failure.

The entrypoint function address will be stored in the pointer pointed to by ```entry_address_ptr```. Will also contain NULL on failure.

The functions returns 0 on success or -1 on failure.

```int bflt_fload(FILE* fp, void **mem_ptr, size_t* mem_size, int (**entry_address_ptr)(int,char*[]));```

Same as above except takes a file pointer as its argument. The file pointer is NOT freed on return.

```void bflt_free(void* ptr);```

Frees memory when finished. Give it the address stored in ```mem_ptr``` - NOT the address in ```entry_point_ptr```.

```void bflt_free_cached();```

Free all cached libraries.

## Loading binaries

See ```load.c``` for more information.

Basically, load the binary into memory and call the entrypoint function pointer with the apropriate values. Then free it when the program finishes executing.

## Getting a elf2flt toolchain

Firstly you need binutils compiled for ARM. Get your binutils from your favourite location.

It's best to let binutils have it's own build directory. ```mkdir binutils-build && cd binutils-build```

```../binutils-src/configure --prefix=/usr/local --target=arm-none-eabi --disable-nls``` and ```make```

Note that you don't actually need to install it. Make a note of the location of the build directory and original source directory.

Next, checkout the source code for elf2flt.

Log into the CVS ```cvs -d:pserver:anonymous@cvs.uclinux.org:/var/cvs login```

Press Enter with no password entered.

Then checkout the source code ```cvs -z3 -d:pserver:anonymous@cvs.uclinux.org:/var/cvs checkout -P elf2flt```

Change into the elf2flt directory and run

```
./configure --target=arm-none-eabi \
--prefix=/usr/local \
--with-libbfd=/path/to/binutils/src/build/directory/libbfd.a \
--with-libiberty=/path/to/binutils/build/directory/libiberty/libiberty.a \
--with-bfd-include-dir=/path/to/binutils/build/directory/bfd \
--with-binutils-include-dir=/path/to/binutils/src/include
```

Replace ```/path/to/binutils/build/directory``` with the path to your build directory and ```/path/to/binutils/src``` with the original binutils source code directory. You may also want to change the prefix if you installed your toolchain elsewhere.

Run ```make``` and ```sudo make install``` and you'll have a working elf2flt toolset.

Finally you need to change into the ```tools``` directory and run ```make```. This will build the nessecary startup files.

To output bFLT executables for your projects, you simply need to change your ```GCC``` and ```LD``` to ```nspire-bflt-gcc``` and ```nspire-bflt-ld``` respectively. You also need to remove the call to ```objcopy```.

The output from ```nspire-bflt-ld``` will be the executable itself.

Here's a sample Makefile

```
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
```