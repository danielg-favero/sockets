#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>

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
    server->sin_addr.s_addr = INADDR_ANY;
	server->sin_family = AF_INET;
	server->sin_port = htons(8888);
}

void bindPortToServer(struct sockaddr_in server, int socketDesc) {
    int binded = bind(socketDesc, (struct sockaddr *)&server, sizeof(server));

	if(binded < 0) {
		puts("Bind failed\n");
	}
    
	puts("Bind done");
}

void acceptConnection(struct sockaddr_in client, int socketDesc) {
    int c = sizeof(struct sockaddr_in);
	int newSocket = accept(socketDesc, (struct sockaddr *)&client, (socklen_t*)&c);

	if (newSocket < 0) {
		perror("Accept failed");
	}
	
	puts("Connection accepted");
}

int main(int argc , char *argv[])
{
	struct sockaddr_in server;
    struct sockaddr_in client;
    

    int socket = createSocket();
    configureServer(&server);
    bindPortToServer(server, socket);
	
	listen(socket , 3);
	
	puts("Waiting for incoming connections...");

	acceptConnection(client, socket);

	return 0;
}