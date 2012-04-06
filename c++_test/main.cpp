/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */

extern "C" {
#include <os.h>
}

class test_class {
    public:
    test_class() {
        printf("test_class constructor called\n");
    }
    int class_method() {
        printf("test_class->class_method called\n");
        return 0;
    }
};

class test_class2 : test_class {
    public:
    test_class2() {
        printf("test_class2 constructor called\n");
    }
    int class_method() {
        printf("test_class2->class_method called\n");
        return 0;
    }
};

int main(int argc, char *argv[]) {
    test_class* obj1 = new test_class;
    test_class2* obj2 = new test_class2;

    obj1->class_method();
    obj2->class_method();

    delete obj1;
    delete obj2;
    return 0;
}
