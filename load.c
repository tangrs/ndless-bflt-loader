/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bflt/bflt.h"
#include "config.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        if (!config_file_already_written()) {
            write_config_file();
        }
        return 0;
    }

    void *bin_mem;
    int (*entry_point)(int, char*[]);
    size_t bin_size;

    if (bflt_load(argv[1], &bin_mem, &bin_size, &entry_point) == 0) {
        clear_cache();
        entry_point(1, (char*[]){ argv[1], NULL });
    }else{
        printf("bflt load did not return 0\n");
    }
    bflt_free(bin_mem);
    bflt_free_cached();
    return 0;
}