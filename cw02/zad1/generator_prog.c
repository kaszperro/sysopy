#include "generator.h"
#include <stdio.h>

int main() {
    generate("super.txt", 10, 10);

    printf("unsorted:\n");
    print_file("super.txt", 10, 10);

    copy_sys("super.txt", "super.sys.txt", 10, 10);
    sort_sys("super.sys.txt", 10, 10);
    printf("sys:\n");
    print_file("super.sys.txt", 10, 10);


    copy_lib("super.txt", "super.lib.txt", 10, 10);
    sort_lib("super.lib.txt", 10, 10);
    printf("lib:\n");
    print_file("super.lib.txt", 10, 10);
    return 0;
}