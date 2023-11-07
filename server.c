#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>

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
    server->sin_addr.s_addr = INADDR_ANY;
	server->sin_family = AF_INET;
	server->sin_port = htons(8888);
}

int bindPortToServer(struct sockaddr_in server, int socketDesc) {
	int opt = 1;
    setsockopt(socketDesc, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

    int binded = bind(socketDesc, (struct sockaddr *)&server, sizeof(server));

	if(binded < 0) {
		perror("Erro ao realizar o Bind\n");
		return 0;
	}
    
	puts("Bind concluído com sucesso");
	return 1;
}

int sendResponse(char *message, int socketDesc) {
	if(write(socketDesc, message, strlen(message)) < 0) {
		perror("\nErro ao enviar resposta ao cliente\n");
		return 0;
	}

	puts("Resposta enviada ao cliente\n");
	return 1;
}

int receiveData(int socketDesc, char *reply) {
    int replyLength = read(socketDesc, reply, MAX_REPLY_LENGTH);

	if(replyLength < 0) {
		perror("Erro ao receber a resposta do cliente\n");
		return 0;
	}

    reply[replyLength] = '\0';

	printf("Cliente respondeu: %s\n", reply);
	return 1;
}

void sendQuestion(int questionId, char* correctAnswer, int socketDesc){ 
	char question[1024];

	strcpy(question, "\n\n#########################\n");

    if(questionId == 1){
        strcat(question, "Qual o planeta mais próximo do sol?\n");
		strcat(question, "Alternativas: \n");
        strcat(question, "1 - Marte \n");
        strcat(question, "2 - Terra \n");
        strcat(question, "3 - Vênus \n");
        strcat(question, "4 - Júpiter \n");
        strcat(question, "5 - Mercúrio \n");
		strcpy(correctAnswer, "5");
    }
    
    else if(questionId == 2){
        strcat(question, "Qual a capital do Brasil?\n");
        strcat(question, "Alternativas: \n");
        strcat(question, "1 - São Paulo \n");
        strcat(question, "2 - Rio de Janeiro \n");
        strcat(question, "3 - Brasília \n");
        strcat(question, "4 - Belo Horizonte \n");
        strcat(question, "5 - Porto Alegre \n");
		strcpy(correctAnswer, "3");
    }
    
    else if(questionId == 3){
        strcat(question, "Qual o resultado de 2 + 2?\n");
        strcat(question, "Alternativas: \n");
        strcat(question, "1 - 4 \n");
        strcat(question, "2 - 5 \n");
        strcat(question, "3 - 6 \n");
        strcat(question, "4 - 7 \n");
        strcat(question, "5 - 8 \n");
		strcpy(correctAnswer, "1");
    }
    
    else if(questionId == 4){
        strcat(question, "Qual o nome do vocalista original da banda Queen?\n");
        strcat(question, "Alternativas: \n");
        strcat(question, "1 - Brian May \n");
        strcat(question, "2 - Freddie Mercury \n");
        strcat(question, "3 - Roger Taylor \n");
        strcat(question, "4 - John Deacon \n");
        strcat(question, "5 - Adam Lambert \n");
		strcpy(correctAnswer, "2");
    }
    
    else if(questionId == 5){
        strcat(question, "Qual o nome do criador do Linux?\n");
        strcat(question, "Alternativas: \n");
        strcat(question, "1 - Bill Gates \n");
        strcat(question, "2 - Steve Jobs \n");
        strcat(question, "3 - Mark Zuckerberg \n");
        strcat(question, "4 - Linus Torvalds \n");
        strcat(question, "5 - Richard Stallman \n");
		strcpy(correctAnswer, "4");
    }

	strcat(correctAnswer, "\0");
	strcat(question, "6 - Sair do Quiz \n");
    strcat(question, "Selecione a alternativa: \0");
    sendResponse(question, socketDesc);
}

void game(int socketDesc) {
	char response[MAX_REPLY_LENGTH];
	int currentQuestionId = 1;
	char correctAnswer[10];

	do {
		sendQuestion(currentQuestionId, correctAnswer, socketDesc);
		if (receiveData(socketDesc, response) && strcmp(response, "6\0") != 0) {
			if (strcmp(response, correctAnswer) == 0){
				sendResponse("\n--------------------\n\nRESPOSTA CORRETA 😁\n\n--------------------\n", socketDesc);
				currentQuestionId++;
			} else {
				sendResponse("\n--------------------\n\nResposta incorreta ❌\n\n--------------------\n", socketDesc);
			}
		}
	} while(strcmp(response, "6\0") != 0 && currentQuestionId <= 5);
}

int main(int argc , char *argv[])
{
	struct sockaddr_in server;
    struct sockaddr_in client;
    int newSocket;
	char *clientIp;
	int clientPort;

    int socket = createSocket();
    configureServer(&server);
    bindPortToServer(server, socket);
	listen(socket , 3);

	puts("Esperando por conexões...");
	
    int c = sizeof(struct sockaddr_in);

    while((newSocket = accept(socket, (struct sockaddr *)&client, (socklen_t *)&c))){
		if (newSocket < 0){
			perror("\nErro ao receber conexao\n");
			return -1;
		}

		clientIp = inet_ntoa(client.sin_addr);
		clientPort = ntohs(client.sin_port);
		printf("Cliente %s se conectou a porta %d\n", clientIp, clientPort);

		game(newSocket);
		
		sendResponse("\n--------------------\n\nQUIZ FINALIZADO!\n\n--------------------\nFinalizando Conexão...\n", newSocket);
		close(newSocket);
    }

	return 0;
}