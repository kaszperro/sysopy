#include "find.h"
#include <stdio.h>
int main() {
    create_table(1);
    set_search_directory(".", "\"*.*\"", "tmp.txt");
    search_directory();
    for(int i = 0; i < 10; ++i) {
        int idx = load_to_array();
        printf("idx: %d\n", idx);
        printf("block: %s", get_block(idx));
        delete_block(idx);
    }
   
    return 0;
}