/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <os.h>
#include "flat.h"
#include "bflt_config.h"
#include "bflt.h"

#define info(f, args...) printf("bFLT: "f"\n", ##args)
#define error_return(x) return (printf("bFLT: "x"\n"), -1)
#define error_goto_error(x) do { printf("bFLT: "x"\n"); goto error; } while(0)

#ifdef SHARED_LIB_SUPPORT

typedef struct {
    void *base;
    unsigned long build_date;
} shared_lib_t;

/* we exclude index 0 because that referres to the current executable - all indexes are one behind */
static shared_lib_t lib_cache[sizeof(shared_lib_t) * (MAX_SHARED_LIB_ID-1)];

#endif

static inline void endian_fix32(uint32_t * tofix, size_t count) {
    /* bFLT is always big endian */

    /* endianness test */
    union {
        uint16_t int_val;
        uint8_t  char_val[2];
    } endian;
    endian.int_val = 1;

    if (endian.char_val[0]) {
        /* we are little endian, do a byteswap */
        size_t i;
        for (i=0; i<count; i++) {
            tofix[i] = bswap32(tofix[i]);
        }
    }

}

static int read_header(FILE* fp, struct flat_hdr * header) {
    fseek(fp, 0, SEEK_SET);
    size_t bytes_read = fread(header, 1, sizeof(struct flat_hdr), fp);
    endian_fix32(&header->rev, ( &header->build_date - &header->rev ) + 1);

    if (bytes_read == sizeof(struct flat_hdr)) {
        return 0;
    }else{
        error_return("Error reading header");
    }
}
static int check_header(struct flat_hdr * header) {
    if (memcmp(header->magic, "bFLT", 4) != 0) error_return("Magic number does not match");
    if (header->rev != FLAT_VERSION) error_return("Version number does not match");

    /* check for unsupported flags */
    if (header->flags & (FLAT_FLAG_GZIP | FLAT_FLAG_GZDATA)) error_return("Unsupported flags detected");

    return 0;
}
#ifdef SHARED_LIB_SUPPORT
static int load_shared_library(int id, void **base, unsigned long current_build_date) {
    FILE* fp = NULL;
    info("Trying to load library id %d", id);
    if (id < 0 || id > 0xfe) error_goto_error("Attempted to load library with invalid ID");
    if (id > MAX_SHARED_LIB_ID) error_goto_error("Library ID too high");

    /* check if library is already loaded */
    if (lib_cache[id-1].base != NULL) {
        if (lib_cache[id-1].build_date > current_build_date) error_goto_error("Library build date is newer than current executable. Refusing to load.");

        info("Returning cached library pointer");
        *base = lib_cache[id-1].base;
        return 0;
    }


    char filename[128];
    sprintf(filename,"%s/lib%d.so.tns",LIB_SEARCH_DIR,id);
    fp = fopen(filename, "rb");
    if (!fp) {
        info("Could not open %s",filename);
        goto error;
    }

    /* get build date */
    struct flat_hdr header;
    if (read_header(fp,&header) != 0) error_goto_error("Could not read library header");
    if (header.build_date > current_build_date) error_goto_error("Library build date is newer than current executable. Refusing to load.");

    int (*entry_point)(int,char*[]);
    size_t dummy;

    /* load into memory - nts: potential circular dependancy problem */
    if (bflt_fload(fp, base, &dummy, &entry_point) != 0) error_goto_error("Could not load library");

    /* initialize the library */
    clear_cache();
    if (entry_point(0,NULL) != 0) info("Warning: Library (ID:%d) init routine returned nonzero",id);

    /* add to lib_cache */
    lib_cache[id-1].base = *base;
    lib_cache[id-1].build_date = header.build_date;

    /* successfully loaded library */
    fclose(fp);
    return 0;
    error:
    *base = NULL;
    if (fp) fclose(fp);
    return -1;
}
#endif /* SHARED_LIB_SUPPORT */

static int copy_segments(FILE* fp, struct flat_hdr * header, void * mem, size_t max_size) {
    /* each segment follows on one after the other */
    /* [   .text   ][   .data   ][   .bss   ]         */
    /* ^entry       ^data_start  ^data_end  ^bss_end  */
    /* that means we can copy them all at once */
    fseek(fp, header->entry, SEEK_SET);

    size_t size_required = header->bss_end - header->entry;
    size_t size_to_copy = header->data_end - header->entry;
    if (size_required > max_size) error_return("Segment buffer not large enough");

    /* zero out memory for bss */
    memset( (char*)mem + size_to_copy, 0, size_required - size_to_copy );

    if (fread(mem, 1, size_to_copy, fp) == size_to_copy) {
        return 0;
    }else{
        error_return("Could not read all segments");
    }
}

static inline void* calc_reloc(uint32_t offset, struct flat_hdr * header, void *base) {
    /* the library id is located in high byte of offset entry */
    int id = (offset >> 24) & 0xff;
    offset &= 0x00ffffff;

    /* fix up offset */
    if (id != 0) {
        /* need to load shared library */
#ifndef SHARED_LIB_SUPPORT
        error_return("No support for bFLT shared libraries");
#else
        if (load_shared_library(id, &base, header->build_date) != 0) return (void*)-1;
#endif
    }
    return (void*)((uint32_t)base + offset);

}

static int process_relocs(FILE *fp, struct flat_hdr * header, void * base) {
    if (!header->reloc_count) { info("No relocation needed"); return 0; }
    fseek(fp, header->reloc_start, SEEK_SET);
    size_t size_to_copy = sizeof(uint32_t) * header->reloc_count;
    uint32_t * offset_list = malloc(size_to_copy);
    if (!offset_list) error_return("Failed to allocate temporary memory in process_relocs");

    size_t size_read = fread(offset_list, sizeof(uint32_t), header->reloc_count, fp);
    if (size_read < size_to_copy/sizeof(uint32_t)) {
        free(offset_list);
        error_return("Failed to read relocs");
    }

    endian_fix32(offset_list, header->reloc_count);

    size_t i;
    for (i=0; i<header->reloc_count; i++) {
        uint32_t fixme = *(uint32_t*)((uint32_t)base + offset_list[i]);

        void* relocd_addr = calc_reloc(fixme, header, base);
        if (relocd_addr == (void*)-1) {
            free(offset_list);
            error_return("Unable to calculate relocation address");
        }

        *(uint32_t*)((uint32_t)base + offset_list[i]) = (uint32_t)relocd_addr;
    }
    free(offset_list);
    return 0;
}
static int process_got(struct flat_hdr * header, void * base) {
    uint32_t *got = (uint32_t*)((uint32_t)base + header->data_start - header->entry);

    for (; *got != 0xffffffff; got++) {
        void* relocd_addr = calc_reloc(*got, header, base);
        if (relocd_addr == (void*)-1) {
            error_return("Unable to calculate got address");
        }
        *got = (uint32_t)relocd_addr;
    }
    return 0;
}

int bflt_load(char* filename, void **mem_ptr, size_t *mem_size, int (**entry_address_ptr)(int,char*[])) {
    FILE* fp = fopen(filename, "rb");
    int ret;
    if (!fp) error_return("Can't open filename");

    ret = bflt_fload(fp, mem_ptr, mem_size, entry_address_ptr);

    fclose(fp);
    return ret;
}

int bflt_fload(FILE* fp, void **mem_ptr, size_t *mem_size, int (**entry_address_ptr)(int,char*[])) {
    void * mem = NULL;
    struct flat_hdr header;

    info("Begin loading");

    if (!fp) error_goto_error("Recieved bad file pointer");
    if (read_header(fp, &header) != 0) error_goto_error("Could not parse header");
    if (check_header(&header) != 0) error_goto_error("Bad header");

    size_t binary_size = header.bss_end - header.entry;
    info("Attempting to alloc %u bytes",binary_size);
    mem = malloc(binary_size);
    if (!mem) error_goto_error("Failed to alloc binary memory");

    if (copy_segments(fp, &header, mem, binary_size) != 0) error_goto_error("Failed to copy segments");
    if (process_relocs(fp, &header, mem) != 0) error_goto_error("Failed to relocate");

    /* only attempt to process GOT if the flags tell us a GOT exists AND
       if the Ndless startup file is not already doing so */
    if (header.flags & FLAT_FLAG_GOTPIC && memcmp(mem, "PRG\0", 4) != 0) {
        if (process_got(&header, mem) != 0) error_goto_error("Failed to process got");
    }else{
        info("No need to process got - skipping");
    }

    *mem_ptr = mem;
    *mem_size = binary_size;

    if (memcmp(mem, "PRG\0", 4) == 0) {
        info("Detected as ndless program packaged in a bFLT file");
        *entry_address_ptr = (int (*)(int,char*[]))((char*)mem + 4);
    }else{
        info("Detected as ordinary bFLT executable");
        *entry_address_ptr = (int (*)(int,char*[]))mem;
    }

    info("Successfully loaded bFLT executable to memory");
    return 0;
    error:
    if (mem) free(mem);
    *mem_ptr = NULL;
    *entry_address_ptr = NULL;
    *mem_size = 0;
    error_return("Caught error - exiting");
}

void bflt_free(void* ptr) {
    info("Free'd bFLT executable");
#ifdef SHARED_LIB_SUPPORT
#ifndef CACHE_LIBS_AFTER_EXEC
    bflt_free_cached();
#endif
#endif
    free(ptr);
}
void bflt_free_cached() {
#ifdef SHARED_LIB_SUPPORT
    info("Free'd bFLT cached libraries");
    size_t i;
    for (i=0; i<sizeof(shared_lib_t) * (MAX_SHARED_LIB_ID-1); i++) {
        free(lib_cache[i].base);
        lib_cache[i].base = NULL;
        lib_cache[i].build_date = 0;
    }
#endif
}

