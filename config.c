/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <os.h>

#define NDLESS_CONFIG_FILE "/documents/ndless/ndless.cfg.tns"
#define CONFIG_LINE "ext.bflt=bflt-loader"

int config_file_already_written() {
    FILE *fp = fopen(NDLESS_CONFIG_FILE,"rb");
    int ret;
    if (!fp) { return 0; }
    struct stat filestat;
    if (stat(NDLESS_CONFIG_FILE,&filestat) == -1) { ret = 0; goto cleanup; }

    void* mem = malloc(filestat.st_size+1);
    if (!mem) { ret = 0; goto cleanup; }

    fread(mem, 1, filestat.st_size, fp);

    ((char*)mem)[filestat.st_size] = 0;

    ret = (strstr(mem, CONFIG_LINE) != NULL);

    cleanup:
    fclose(fp);
    free(mem);

    return ret;
}

void write_config_file() {
    FILE *fp = fopen(NDLESS_CONFIG_FILE,"ab");
    if (!fp) { return; }
    fwrite("\n"CONFIG_LINE"\n", 1, sizeof("\n"CONFIG_LINE"\n")-1, fp);
    fclose(fp);
}