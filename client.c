#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

int createSocket() {
    int socketDesc;

    socketDesc = socket(AF_INET, SOCK_STREAM, 0);

    if(socketDesc == -1) {
        printf("Could not create socket\n");
        return -1;
    }

    return socketDesc;
}

void configureServer(struct sockaddr_in *server) {
    server->sin_addr.s_addr = inet_addr("172.31.3.109");
	server->sin_family = AF_INET;
	server->sin_port = htons(8888);
}

int connectServer(struct sockaddr_in server, int socketDesc) {
    int connected = connect(socketDesc , (struct sockaddr *)&server , sizeof(server));

	if (connected < 0) {
		puts("Connection error\n");
		return 1;
	}

    puts("Connected");
}

int sendData(char *message, int socketDesc) {
    int sended = send(socketDesc, message, strlen(message), 0);

	if(sended < 0) {
		puts("Send failed\n");
		return 1;
	}

	puts("Data Send\n");
}

int receiveData(int socketDesc) {
    int replyLength = 2000;
    char reply[replyLength];

    int received = recv(socketDesc, reply, replyLength, 0);

	if(received  < 0) {
		puts("Data receiving failed\n");
	}

	puts("Reply received\n");
	puts(reply);
}

int main() {
    struct sockaddr_in server;

    int socket = createSocket();
    configureServer(&server);
    connectServer(server, socket);

    sendData("GET / HTTP/1.1\r\n\r\n", socket);
    receiveData(socket);

    close(socket);

    return 0;
}