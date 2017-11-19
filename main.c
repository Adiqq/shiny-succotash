#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

void stripNewLine(char *text){
    size_t length = strlen(text);
    if(length > 0 && text[length - 1] == '\n') text[length - 1] = '\0';
    if(length > 1 && text[length - 2] == '\r') text[length - 2] = '\0';
}

int main(void)
{
    const ushort PORT = 1024;
    const ushort BUFFER_SIZE = 255;

	int fd = socket(PF_INET, SOCK_STREAM, 0);
    int on = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));

	struct sockaddr_in addr;
	addr.sin_family = PF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(fd, (struct sockaddr*)&addr, sizeof(addr)) != 0){
        fprintf(stderr, "Value of errno: %d\n", errno);
        fprintf(stderr, "Error bind: %s\n", strerror( errno ));
        exit(-1);
    }
    if(listen(fd, 10) != 0){
        fprintf(stderr, "Value of errno: %d\n", errno);
        fprintf(stderr, "Error bind: %s\n", strerror( errno ));
        exit(-1);
    };
    struct sockaddr_in client_addr;
    socklen_t client_addr_length;
    while(1){
        printf("Waiting for connection...\n");
        int clientfd = accept(fd, (struct sockaddr*)&client_addr, &client_addr_length);
        if(clientfd == -1){
            fprintf(stderr, "Value of errno: %d\n", errno);
            fprintf(stderr, "Error accepting: %s\n", strerror( errno ));
            exit(-1);;
        };
        printf("Accepted connection...\n");
        printf("Client fd: %d\n", clientfd);
        char buf[BUFFER_SIZE];
        char send_buf[BUFFER_SIZE];
        if(read(clientfd, buf, BUFFER_SIZE) != -1){
            printf("Striping newline, length before:%lu\n", strlen(buf));
            stripNewLine(buf);
            printf("Stripped newline, length after:%lu\n", strlen(buf));

            printf("Writing response...\n");
            if(strcmp(buf,"129027") == 0){
                strcpy(send_buf,"Adrian Wawrzak\n");
                write(clientfd, send_buf, strlen(send_buf));
            } else if(strcmp(buf,"129015") == 0){
                strcpy(send_buf,"Mikołaj Rekruciak\n");
                write(clientfd, send_buf, strlen(send_buf));
            } else {
                strcpy(send_buf,"Lama exception - incorrect index, try again...\n");
                write(clientfd, send_buf, strlen(send_buf));
            }
        } else{
            printf("Writing error response...\n");
            strcpy(send_buf,"Read error");
            write(clientfd, send_buf, strlen(send_buf));
        }
        close(clientfd);
    }


    close(fd);
	return 0;
}