#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <string.h>

int file_info(const char *filename, const struct stat *statptr,
    int fileflags, struct FTW *pfwt) {
  
    char file_type[64] = "undefined";

    if(fileflags & FTW_F) {     
        strcpy(file_type, "file");
    } 
    else if(fileflags & FTW_D) {
        strcpy(file_type, "directory");
    }
    else if(fileflags & FTW_SL) {
        strcpy(file_type, "symbolic link");
    }

    printf("%s || type: %s, size: %d, last a time: %d, last m time: %d\n", filename, file_type, statptr->st_size, statptr->st_atime, statptr->st_mtime);

    return 0;
}

int search_dir(char *dirpath) {
    int fd_limit = 5;
    int flags = FTW_CHDIR | FTW_DEPTH | FTW_MOUNT | FTW_PHYS;
 
    return nftw(dirpath,
        file_info,
        fd_limit, flags);
}

int main(int argc, char *argv[]) {

    search_dir(argv[1]);

    return 0;
}
