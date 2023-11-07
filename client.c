#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define MAX_REPLY_LENGTH 2000

int createSocket() {
    int socketDesc;

    socketDesc = socket(AF_INET, SOCK_STREAM, 0);

    if(socketDesc == -1) {
        perror("Não foi possível criar o socket\n");
        return -1;
    }

    return socketDesc;
}

void configureServer(struct sockaddr_in *server) {
    server->sin_addr.s_addr = inet_addr("172.20.94.39");
	server->sin_family = AF_INET;
	server->sin_port = htons(8888);
}

int connectServer(struct sockaddr_in server, int socketDesc) {
    int connected = connect(socketDesc , (struct sockaddr *)&server , sizeof(server));

	if (connected < 0) {
		perror("Erro de conexão\n");
		return 1;
	}

    puts("Conectado");
}

int sendData(char *message, int socketDesc) {
    int sended = send(socketDesc, message, strlen(message), 0);

	if(sended < 0) {
		puts("\nErro ao enviar opção ao servidor, tente novamente...\n");
		return 0;
	}

	puts("\nOpção enviada ao servidor\n");
    return 1;
}

int receiveData(int socketDesc, char *reply) {
    int replyLength = recv(socketDesc, reply, MAX_REPLY_LENGTH, 0);

	if(replyLength < 0) {
		perror("Erro ao receber a resposta do servidor\n");
        return 0;
	}

    reply[replyLength] = '\0';

	printf("Servidor respondeu: %s\n", reply);
    return 1;
}

int main() {
    struct sockaddr_in server;
    char option[4];
    char question[MAX_REPLY_LENGTH];
    char feedback[MAX_REPLY_LENGTH];

    int socket = createSocket();
    configureServer(&server);
    connectServer(server, socket);

    do {
        receiveData(socket, question);

        scanf("%s", &option);
        sendData(option, socket);
        fflush(stdin);

        receiveData(socket, feedback);

        if(strstr(feedback, "Close") != NULL) break;
    } while(strcmp(option, "6\0") != 0);

    close(socket);

    return 0;
}