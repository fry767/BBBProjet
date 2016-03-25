
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <syslog.h>
#include <time.h>
#include <pthread.h>
#include "../include/baborwire.h"

#define PORT_NUM_APP    9202
#define SERV_IP_ADDR    "192.168.49.33"
#define RX_BUF_SIZE     7

#define DEBUG			1

enum {FALSE, TRUE};

#define SOF_AA	0
#define SOF_55	1
#define CMD_ID	2
#define DATA	3

#define print(prio, fmt, ...) \
do  { \
	if (DEBUG) printf(">> " fmt "\n\n", __VA_ARGS__); \
	else syslog(prio, fmt, __VA_ARGS__); \
	} while (0)


extern uint32_t vitesse;
int delay_ms(long ms)
{
    time_t s;
    long ns;

    s = ms / 1000;
    ns = (ms % 1000) * 1000 * 1000;

    nanosleep((struct timespec[]){{s, ns}}, NULL);

    return 0;
}

void add_data(unsigned char *buf, int data)
{
	buf[DATA + 0] = data >> 24;
	buf[DATA + 1] = data >> 16;
	buf[DATA + 2] = data >> 8;
	buf[DATA + 3] = data;
}

void *handler_comm(void *arg)
{

	unsigned char buffer[RX_BUF_SIZE];
	char client_addr[INET_ADDRSTRLEN];
    int sockfd_server_app, sockfd_client_app;
    int addrlen;
    struct sockaddr_in serv_for_app, client;    
    int bytesReceived;
    int i, optval;
	unsigned int command_id, data;
    
    //daemon(0, DEBUG);
    
    print(LOG_INFO, "%s", "server is started!");
    
    /* Set connexions configuration */
    memset(&serv_for_app, 0, sizeof(struct sockaddr_in));
    serv_for_app.sin_family  = AF_INET;
    serv_for_app.sin_port    = htons(PORT_NUM_APP);
    if (inet_pton(AF_INET, SERV_IP_ADDR, &serv_for_app.sin_addr) == 0) {
        print(LOG_ERR, "error with inet_pton() for app : %s", strerror(errno));
        pthread_exit(&errno);
    }    
    
    /* Create sockets */   
    sockfd_server_app = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_server_app == -1) {
        print(LOG_ERR, "error with socket() for app : %s", strerror(errno));
        pthread_exit(&errno);
    }
    
    /* Set sockets options */
    optval = 1;  
    if (setsockopt(sockfd_server_app, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1){
        print(LOG_ERR, "error with setsockopt() for app : %s", strerror(errno));
        pthread_exit(&errno);
    }
    
    /* Bindind sockets */    
    if (bind(sockfd_server_app, (const struct sockaddr *)&serv_for_app, sizeof(struct sockaddr_in)) == -1) {
        print(LOG_ERR, "error with bind() for app : %s", strerror(errno));
        pthread_exit(&errno);
    }
    
    /* Set sockets in listening state */  
    if (listen(sockfd_server_app, 1) == -1) {
        print(LOG_ERR, "error with listen() for app : %s", strerror(errno));
        pthread_exit(&errno);
    }
    
    /* Wait for the telemetry app to connect and authenticate */
    print(LOG_INFO, "%s", "socket for app is now accepting connexions");
    addrlen = sizeof(struct sockaddr);
    sockfd_client_app = accept(sockfd_server_app, (struct sockaddr *)&client, &addrlen);
    if (sockfd_client_app == -1) {
        print(LOG_ERR, "error with accept() for app : %s", strerror(errno));
        pthread_exit(&errno);
    }
	inet_ntop(AF_INET, &client.sin_addr, client_addr, INET_ADDRSTRLEN);
	
	print(LOG_INFO, "client with ip address %s is connected", client_addr);
    
    print(LOG_INFO, "%s", "app has been authenticated successfully");
    
    do {
        bytesReceived = recv(sockfd_client_app, &buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
			//for (i = 0; i < bytesReceived; i++) {
				//print(LOG_INFO, "buffer[%d]: 0x%.2X", i, buffer[i], buffer[i]);
			//}
			if (buffer[SOF_AA] == 0xAA) {
				if (buffer[SOF_55] == 0x55) {
					command_id = buffer[CMD_ID];
					data = *((unsigned int *)(&buffer[DATA]));
					switch (command_id) {
						case 0x01: // (input) Longeur du câble
							//print(LOG_INFO, "Command #%d received!", command_id);
							print(LOG_INFO, "Cable Length = %d", data);
							break;
						case 0x02: // (input) Vitesse
							//print(LOG_INFO, "Command #%d received!", command_id);
							printf("Vitesse = %.3d Km/h\r", data);
							vitesse = data;
							break;
						case 0x03: // (output) Temperature de la batterie
							print(LOG_INFO, "Command #%d received!", command_id);
							add_data(buffer, 0x03);
							send(sockfd_client_app, &buffer, 7, 0);
							break;
						case 0x04: // (output) Charge de la batterie
							print(LOG_INFO, "Command #%d received!", command_id);
							add_data(buffer, 0x04);
							send(sockfd_client_app, &buffer, 7, 0);
							break;
						case 0x05: // (output) Position du charriot
							print(LOG_INFO, "Command #%d received!", command_id);
							add_data(buffer, 0x05);
							send(sockfd_client_app, &buffer, 7, 0);
							break;
						case 0x06: // (output) Vitesse réelle
							print(LOG_INFO, "Command #%d received!", command_id);
							add_data(buffer, 0x06);
							send(sockfd_client_app, &buffer, 7, 0);
							break;
					}
				}
			}

            //print(LOG_INFO, "buffer[%d]: %c - %x", i, buffer[i], buffer[i]);
            //send(sockfd_client_app, &buffer, bytesReceived, 0);
        }
    } while(bytesReceived > 0);

    print(LOG_INFO, "%s", "server is terminated");

    pthread_exit(NULL);	
}

//int main(int argc, char *argv[])
//{
//	pthread_t thread_comm;
//	int rc;
//
//	/* disable stdout buffering */
//	setvbuf(stdout, NULL, _IONBF, 0);
//
//	if ((rc = pthread_create(&thread_comm, NULL, handler_comm, NULL))) {
//      fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
//      return EXIT_FAILURE;
//    }
//
//	pthread_join(thread_comm, NULL);
//}
