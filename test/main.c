#include <os.h>

typedef void (foofunc)();

void foo() { printf("hello"); }

char *globvar = "Hello from global!\n";

void (*globfunc)() = foo;

foofunc *globarray[] = {
    foo,
    foo,
    foo
};

int main(int argc, char *argv[]) {
    static foofunc *localfunc = foo;
    static foofunc *staticarray[] = {
        foo,
        foo,
        foo
    };
    int globarray_test_successes = 0, staticarray_test_successes = 0, i;

    printf("Unit test results\n");
    printf("globvar = %p, expected = N/A, value = %s\n", (void*)globvar, globvar);

    printf("globfunc = %p, expected = %p\n", (void*)globfunc, (void*)foo);
    printf("localfunc = %p, expected = %p\n", (void*)localfunc, (void*)foo);

    for (i=0; i<sizeof(globarray)/sizeof(foofunc*); i++) {
        if (globarray[i] == &foo) globarray_test_successes++;
        printf("globarray[%d] = %p, expected = %p\n", i, (void*)(globarray[i]), (void*)foo);
    }

    for (i=0; i<sizeof(staticarray)/sizeof(foofunc*); i++) {
        if (staticarray[i] == &foo) staticarray_test_successes++;
        printf("staticarray[%d] = %p, expected = %p\n", i, (void*)(staticarray[i]), (void*)foo);
    }


    if (
        (void*)globfunc == (void*)foo &&
        (void*)localfunc == (void*)foo &&
        globarray_test_successes == sizeof(globarray)/sizeof(foofunc*) &&
        staticarray_test_successes == sizeof(globarray)/sizeof(foofunc*)
        ) {
        printf("Unit test successful\n");
        return 0;
    }else{
        printf("Unit test failed\n");
        return -1;
    }
}
