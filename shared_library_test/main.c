#include <os.h>
#define global_var_string "Hello from global!"
#define perform_cmp(var, expected) printf(#var " = %p, expected = %p  [%s]\n", (void*)(var), (void*)(expected), (var == expected ? "PASS" : ((fail = 1), "FAIL")))

typedef void (foofunc)();
void foo() { }

char *global_var = global_var_string;
char **global_ptr_to_ptr = &global_var;

foofunc *global_func_ptr = foo;
foofunc *global_func_ptr_array[] = {
    foo,
    foo,
    foo
};

static volatile unsigned char static_zeros[64];

int main(int argc, char *argv[]) {
    static foofunc *static_func_ptr = foo;
    static foofunc *static_func_ptr_array[] = {
        foo,
        foo,
        foo
    };
    int fail = 0;
    unsigned i;

    printf("Unit test results\n");
    printf("global_var = %p, value = %s, expected value = %s [%s]\n", (void*)global_var, global_var, global_var_string,
        (strncmp(global_var, global_var_string, sizeof(global_var_string)) == 0 ? "PASS" : ((fail = 1), "FAIL")));

    perform_cmp(global_ptr_to_ptr, &global_var);

    perform_cmp(global_func_ptr, foo);
    perform_cmp(static_func_ptr, foo);

    for (i=0; i<sizeof(global_func_ptr_array)/sizeof(foofunc*); i++) {
        perform_cmp(global_func_ptr_array[i], foo);
    }

    for (i=0; i<sizeof(static_func_ptr_array)/sizeof(foofunc*); i++) {
        perform_cmp(static_func_ptr_array[i], foo);
    }

    printf("Checking if .bss is correctly initialized...");
    int bss_fail = 0;
    for (i=0; i<sizeof(static_zeros) ;i++) {
        printf("%s%u ", (i % 8 == 0) ? "\n" : "", static_zeros[i]);
        if (static_zeros[i] != 0) { bss_fail = 1; fail = 1; }
    }
    if (bss_fail) printf("[FAIL]\n"); else printf("[PASS]\n");

    int library_call();
    printf("Calling library function [%s]\n", library_call() ? "FAIL" : "PASS");

    if (!fail) {
        printf("Unit test successful\n");
        return 0;
    }else{
        printf("Unit test failed\n");
        return -1;
    }
}
