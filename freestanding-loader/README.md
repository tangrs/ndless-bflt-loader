# Freestanding bFLT loader

This is a tiny bFLT loader that does in-place relocation with no support for shared libraries. It's aim is to be very small but even still, it takes ~252bytes at least.

This could be useful for backwards compatibility.

## Using

Using this loader is easy. Simply ```cat``` your binary on top of the loader.bin file.

I.e. ```cat loader.bin /path/to/program.bflt.tns > patched.tns```

## Restrictions

The BSS must not be longer than the total number of relocations. Relocations are zero'd out at the end of the file to serve a second purpose as the BSS.