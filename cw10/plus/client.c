#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>  
#include <unistd.h>

#define PORT 8080 
   
int main(int argc, char const *argv[]) { 
    int sock = 0; 
    struct sockaddr_in serv_addr; 
    char *hello = "Hello from client"; 
    char buffer[1024] = {0}; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)  { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 

    while(read( sock , buffer, 1024) ) {
        printf("%s\n",buffer );
        send(sock, hello, strlen(hello), 0);
    }


    close(sock);
    return 0; 
} 