#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024
/*
//send
if (send(client_socket, message, strlen(message), 0) == -1) {
        perror("Errore nell'invio dei dati al server");
        exit(EXIT_FAILURE);
}
//recv
if (recv(client_socket, buffer, sizeof(buffer), 0) == -1) {
        perror("Errore nella recv dal server");
        exit(EXIT_FAILURE);
}
*/ 
int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Creazione del socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Errore nella creazione del socket");
        exit(EXIT_FAILURE);
    }

    // Inizializzazione della struttura del server
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Connessione al server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Errore nella connessione al server");
        exit(EXIT_FAILURE);
    }
//////////////////////////////////////////////////////////////////////////////////////////
//                  inizio programma
    printf("Connesso al server %s:%d\n", SERVER_IP, PORT);
    
    // Invio dei dati al server
    char message[BUFFER_SIZE];
    int lenBuffer=0;
    if ((lenBuffer=recv(client_socket, buffer, sizeof(buffer), 0)) == -1) {
            perror("Errore nella recv dal server");
            exit(EXIT_FAILURE);
    }
    
    buffer[lenBuffer]=0;
    while (1)
    {   
        char input[BUFFER_SIZE];
        printf("%s\n",buffer);
        if(strcmp(buffer,"benvenuto")==0){
            printf("Prova ad indovinare un numero: \n");
            scanf("%s",input);
        }else if(strcmp(buffer,"ALTO")==0){
            printf("Il numero inserito è troppo alto: \n");
            scanf("%s",input);
        }else if(strcmp(buffer,"BASSO")==0){
            printf("Il numero inserito è troppo basso: \n");
            scanf("%s",input);
        }else{
            printf("Hai indovinato il numero!\n");
            break;
        }

        if (send(client_socket, input, strlen(input), 0) == -1) {
            perror("Errore nell'invio dei dati al server");
            exit(EXIT_FAILURE);
        }

        // Ricezione dei dati dal server
        memset(buffer, 0, sizeof(buffer));
        if ((lenBuffer=recv(client_socket, buffer, sizeof(buffer), 0) )== -1) {
            perror("Errore nella recv dal server");
            exit(EXIT_FAILURE);
        }
        buffer[lenBuffer]=0;

    }
    

    // Chiusura della connessione
    close(client_socket);

    return 0;
}
