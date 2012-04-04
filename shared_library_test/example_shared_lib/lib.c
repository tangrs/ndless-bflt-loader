/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */

#include <os.h>

int main() {
    printf("Library id %d initialized\n", LIB_ID);
    return 0;
}

int library_call() {
    printf("Library id %d called!\n", LIB_ID);
    return 0;
}