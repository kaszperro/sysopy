#define _XOPEN_SOURCE 500

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
 #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>


void my_siginit(int sig, siginfo_t * sinf, void * ctx ) {
    printf("jestem");
    
    sleep(1);
    exit(1);
}



int main() {


    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = my_siginit;

    sigaction(SIGINT, &sa, NULL);

    FILE *f = fopen("cos.txt", "w+");
    char t [] = "jakeis super";
    fwrite( t, sizeof (t), 1, f);
    fclose(f);

    int fd = open("cos2.txt", O_CREAT | O_TRUNC | O_RDWR, 0644);
    write(fd, "jakies super", 100);
    
    close(fd);

    

    while(1){
        sleep(1);
    }
    
    return 0;
}