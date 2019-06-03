#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>

#define PORT 8080 

 int server_fd; 
  int new_socket;


int main(int argc, char const *argv[]) {
   
    
   
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
        perror("socket failed"); 
        exit(1); 
    } 
    /*
    int opt = -1;
     if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
  */
    struct sockaddr_in address; 
    int addrlen = sizeof(address); 

    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       

    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) { 
        perror("bind failed"); 
        exit(1); 
    } 

    if (listen(server_fd, 3) < 0) { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 

    
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen))<0)  { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } 
 
    char *ping = "ping from server"; 

    char buffer[1024] = {0}; 
    for(int i = 0; i < 10; ++i) {
        if(send(new_socket, ping, strlen(ping), 0) != -1) {
            printf("sent ping\n");
        } else {
            printf("cant send ping\n");
        }

        int r = recv(new_socket, buffer, 1024, 0);
        if(r != -1 && r != 0) {
            printf("received from client: %s %d\n", buffer, r);
        } else {
            printf("client disconeted\n");
            break;
        }

        sleep(1);
    }



    close(server_fd);
    return 0; 
} 