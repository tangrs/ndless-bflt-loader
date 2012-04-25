# Simple bFLT loader for Ndless

Loads bFLT binaries into memory for Ndless.

## Status

* Can run binaries
* Basic unit tests pass
* No support for gzipped data yet
* Relocations all tested and working correctly
* Shared library support is basically stable (albeit with nasty workarounds)
* Has an toolchain! (toolchain source code can be found at https://github.com/tangrs/ndless-bflt-toolchain)

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

## Toolchain

A toolchain can be found at https://github.com/tangrs/ndless-bflt-toolchain

## License

See the LICENSE file in each folder to see what the files in that folder are licensed under. The file header also indicates the license used for that particular file.

Any unmarked file is licensed under the Mozilla Public License.