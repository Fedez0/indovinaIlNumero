#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <sys/select.h>

#define SERVICEADDR 0x7f000001
#define BUFLEN  100000

#define MAX_CLIENT 10


// indirizzo del server (localhost -> 127.0.0.1) espresso come intero a 32 bit
#define SERVICEPORT 8080 //porta del server non standard
char SERVICENAME[] = "127.0.0.1";    //oppure char SERVICENAME[] = "www.localhost";
 
/*
recv
if ((rcvlen = recv(listaSocket[i].sock,rcvbuf, BUFLEN,0)) < 0) {                        
    error(NULL);
    exit (2);
}
send
if (send (listaSocket[i].sock, sendbuf, strlen(sendbuf), 0) < 0) {
    error(NULL);
    exit (2);
} 

*/
void error(char *msg);
int main () {
    struct sockaddr_in local,remote;
    int s, sc;
    int i;
    unsigned int rcvlen;
    char rcvbuf [BUFLEN], sendbuf[BUFLEN];
    unsigned int remotelen;

    int random[MAX_CLIENT];

	
	fd_set set;  						//definisco var set di tipo fd_set
	
	static struct connection {			//creo struct per contenere dati dei socket dei client connessi
	    int sock;
	    //.......							//aggiungere alla struct le var  da associare a ogni client
	  
	} listaSocket [MAX_CLIENT];
  
	
    for(i=0;i<MAX_CLIENT;i++){		//azzero la lista socket
		listaSocket[i].sock=-1;		//inizializzo l'array con  -1 per poi controllare facilmente se lo slot è libero 
	}
	
	
    printf("Inizializzando il Server!\n");
    
    //////////////////////   crea socket   /////////////////////

    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror ("Creazione Socket: ");
        exit (1);
    }

    printf("Socket creato\n\n");
    
    /////////// compila indirizzo del server (il proprio) e fa la bind ////////
    
    local.sin_family = AF_INET;
    local.sin_port = htons(SERVICEPORT);
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind (s, (struct sockaddr*)&local, sizeof(local)) < 0) {
        perror ("errore Bind: ");
        close(s);
        exit (2);
    }

    printf ("Bind corretta\n\n");

    //////////// ricevo richiesta dal client //////////////////////
    
    if (listen(s, MAX_CLIENT) < 0) {
        error(NULL);
    }
  
	printf("Sono in ascolto....\n\n");
    srand(time(NULL));
     
    do{
            
        FD_ZERO(&set);               // Svuota il set
        FD_SET(s,&set);      		 // Ci aggiunge il socket in listen s

        for (i = 0;i < MAX_CLIENT; i++) {
            if (listaSocket[i].sock >= 0) {	// aggiungo anche tutti i socket client (sc) che sono attivi
                FD_SET(listaSocket[i].sock, &set); 
            }  
        }
        
        //SELECT()
        
        if (select(FD_SETSIZE, &set, NULL, NULL, NULL)<0){   //ritorna i set che hanno un evento
            error(NULL);
        }
        
        if (FD_ISSET(s,&set)) {		//se ho un evento su s, cioè un socket che ha chiesto connessione
            remotelen = sizeof(remote);
            
            sc = accept(s, (struct sockaddr *)&remote, &remotelen);
            
            
            //aggiungo sc alla lista se c'è posto
            
            for (i = 0;i < MAX_CLIENT; i++) {
                if (listaSocket[i].sock == -1) {	// in uno slot libero metto sc 
                    printf("pos %d - Accetto richiesta di connessione di sc = %d n\n", i,sc);
                    listaSocket[i].sock = sc;
                    //......							//azzero var della struct
                    random[i]=(rand()%99)+1;
                    strcpy(sendbuf,"benvenuto");
                    if (send (sc, sendbuf, strlen(sendbuf), 0) < 0) {
                        error(NULL);
                        exit (2);
                    }
                    break;
                }
            }
            
            //se non c'è posto mando "BUSY" e chiudo connessione sc 
            
            if (i ==MAX_CLIENT) {
                
                printf("\nClient nuovo, nessun posto libero\n\n");
                strcpy (sendbuf, "BUSY");   
                printf ("Invio messaggio al client %s\n", sendbuf);  
                if (send (sc, sendbuf, strlen(sendbuf), 0) < 0) {
                    error(NULL);
                    exit (2);
                }
                close(sc);
            
            }
        
        }
        
        
        // aggiorno la lista effettuando la recv di tutti i socket sc in lista
        
            for (i = 0;i < MAX_CLIENT; i++) {
                if (FD_ISSET(listaSocket[i].sock,&set)) {	// Dato sul socket
                    //lavoro da fargli svolgere
                    
                    // Ricevo
                
                    if ((rcvlen = recv(listaSocket[i].sock,rcvbuf, BUFLEN,0)) < 0) {
                        
                        error(NULL);
                        exit (2);
                    }
                    
                    rcvbuf[rcvlen] = 0; // Termina la stringa
                    int risposta=atoi(rcvbuf);
                    
                    
                    printf("\nRicevo: %sda sock %d \n", rcvbuf, listaSocket[i].sock);
                    
                    
                    if(random[i]==risposta){
                        strcpy(sendbuf,"INDOVINATO");
                    }else{
                        if(risposta>random[i]){
                            strcpy(sendbuf,"ALTO");
                        }else{
                            strcpy(sendbuf,"BASSO");
                        }
                    }
                    
                    //e La invio
                    
                    if (send (listaSocket[i].sock, sendbuf, strlen(sendbuf), 0) < 0) {
                        error(NULL);
                        exit (2);
                    } 
                    
                    if(random[i]==risposta){
                        listaSocket[i].sock=-1;
                        close(listaSocket[i].sock);	
                    }
                    //libero posto in lista solo quando mando l'ultima send() al client
                    
                }
            
        }  // for (i = 0;i < MAX_CLIENT; i++) 
    
    } while(1);  // il server non termina
    
    
    close (s);    
    return 0;
}
void error(char *msg){
    perror(msg);
    exit(-1);
}
