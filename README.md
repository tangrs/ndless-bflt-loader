# Simple bFLT loader for Ndless

Loads bFLT binaries into memory for Ndless.

## Status

* Can run binaries

* Basic unit tests pass

* No support for gzipped data yet

* GOT relocation untested (is there a good way to test??)

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

Frees memory when finished. Give it the address stored in ```mem_ptr``` - NOT ```entry_point_ptr```.

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

Finally run ```make``` and ```sudo make install```.

Lastly, you need to modify Ndless's linker script (located in ```trunk/system/ldscript```) to place the ```.data``` section before the ```.bss``` section or else you're going to get linker errors.

It should look something like this:

```
/* See http://sourceware.org/binutils/docs-2.20/ld/Scripts.html#Scripts */

/* Avoid the useless default padding between sections */
SECTIONS
{
	. = 0x0;
	.text : { *(.text); }
	.data : {
		*(.data);
		/* symbols used by asm statements in Ndless macros, optimized out by GCC, we want to emit only if used */
		. = ALIGN(4);
		PROVIDE(_syscallvar_savedlr = .);
		. += 4;
	}
	.bss : {
		/* symbol required by newlib */
		__bss_start__ = .;
		*(.bss);
		__bss_end__ = .;
	}
}
__got_size = SIZEOF(.got) / 4;
__exidx_start = .;
__exidx_end = .;
```

Now you can link into bFLT files.

All you need to do now is add ```-Wl,-elf2flt``` to your LDFLAGS and change some file extensions and get rid of the final objcopy after linking.

Here's a sample Makefile

```
GCC = nspire-gcc
LD = nspire-ld
GCCFLAGS = -elf2flt -Os -nostdlib -Wall -W -marm -mcpu=arm926ej-s
LDFLAGS = -Wl,-elf2flt -nostdlib
EXE = test.tns
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
DISTDIR = .
vpath %.tns $(DISTDIR)

all: $(EXE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

$(EXE): $(OBJS)
	$(LD) $^ -o $(@:.tns=.bflt.tns) $(LDFLAGS)

clean:
	rm -f *.o *.elf
	rm -f $(DISTDIR)/$(EXE)
```