# bFLT shared libraries

I think this section needs some explaining since there's a lot to it.

## Writing libraries

bFLT doesn't explicitly differentiate between libraries and executables. To the loader, they're exactly the same (except the filename).

That means libraries also have a ```main()``` function. This is called by the loader only once per library load. The purpose is to let the library run initilization code. Keep it short and sweet.

Apart from that, writing libraries is the same as writing normal executables.

Library files should be placed in ```LIB_SEARCH_DIR``` which is defined in ```bflt/bflt_config.h```.

## How to create shared libraries

There's a few steps to creating shared libraries. We basically need to produce 3 files at the end:

```lib$(LIBID).so.tns.gdb```: This contains all the information needed to link binaries with.

```_lib$(LIBID)_wraps.o```: The wrappers for that library. This is to implement a workaround for shared library support (see below).

```lib$(LIBID).so.tns```: The actual shared library. This is the only file that needs to be distributed if people want to use it. The others are all for compile time linking.


Of course, we also need some input files. You need:

* Your C files (duh)
* A ```exports.sym``` file containing a newline-separated list of symbols you want to export. All other symbols will be made local and not available to link with executables

Once you have these files, you can add them to the sample Makefile included in this directory.

More specific details can be found in the comments of the Makefile.

## How to use shared libraries

You will need two files: ```lib$(LIBID).so.tns.gdb``` and ```_lib$(LIBID)_wraps.o```.

The process is very simple. Link you binary as you would for normal bFLT binaries while adding a ```-Wl,-R,lib$(LIBID).so.tns.gdb _lib$(LIBID)_wraps.o``` to your ```LDFLAGS``` for every shared library you link to (replacing $(LIBID) with the ID of the library).

Expecting more? Sorry to disappoint.

## The big "code veneering" workaround

A code veneer is automatically inserted by the linker for all weak function references (read: shared library function calls). This acts like a bridge between the binary and shared library and allows the linker to get an offset for the function before the actual symbol is available.

Here's an example of a code veneer disassembly:

```
000008d0 <__library_call_veneer>:
 8d0:   e51ff004    ldr pc, [pc, #-4]   ; 8d4 <__library_call_veneer+0x4>
 8d4:   03000320    .word   0x03000320  ; This address isn't correctly relocated!
```

Unfortunately for us, the address it branches to isn't relocated correctly at runtime for some reason. The linker doesn't seem to view it as something it needs to relocate and so, doesn't come up in the list of relocations. This means the run time loader doesn't know anything about relocating it.

Of course, calling that function will result in a crash as it tries to branch to address 0x03000320.

The workaround for this was to skip the veneer altogether and basically write our own. This is done by taking advantage of the linker's --wrap parameter which redirects symbols. The Makefile simply generates a wrapper assembly file containing symbols from the library that loads addresses that ARE relocated correctly at run time.

This section is kept in the .data section of the file since it doesn't relocate correctly if I place it in the .text section. The Nspire doesn't differentiate between code and data so this works like a charm.