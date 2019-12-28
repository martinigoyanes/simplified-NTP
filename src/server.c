#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind, and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() and getpid() */
#include <fcntl.h>      /* for fcntl() */
#include <sys/file.h>   /* for O_NONBLOCK and FASYNC */
#include <signal.h>     /* for signal() and SIGALRM */
#include <errno.h>      /* for errno */
#include "assigment1.h"
#include "optparser.h"
#include "Practical.h"
#include <time.h>


short max_seqnum = 1;
void CatchAlarm(int ignored);  
int main(int argc, char *argv[])
{
	
	struct server_arguments args = server_parseopt(argc, argv); 
	
	struct timespec *time = malloc(sizeof(struct timespec));
	struct timerequest request;
	struct timeresponse response;
	
    struct sockaddr_in ServAddr; /* Server address */
    unsigned short ServPort;     /* Server port */
    
	struct sockaddr_in ClntAddr;  /* Address of datagram source */
    unsigned int clntLen;             /* Address length */
    ssize_t numbytes;                  /* Size of datagram */
    int sock;                        /* Socket */
    
    struct sigaction myAction;       /* For setting signal handler */
	
	 /* Set signal handler for alarm signal */
    myAction.sa_handler = CatchAlarm;
    if (sigfillset(&myAction.sa_mask) < 0) /* block everything in handler */
        DieWithSystemMessage("sigfillset() failed");
    myAction.sa_flags = 0;

    if (sigaction(SIGALRM, &myAction, 0) < 0)
        DieWithSystemMessage("sigaction() failed for SIGALRM");
  
    
    ServPort = args.port;  /* First arg:  local port */

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithSystemMessage("socket() failed");

    /* Set up the server address structure */
    memset(&ServAddr, 0, sizeof(ServAddr));   /* Zero out structure */
    ServAddr.sin_family = AF_INET;                /* Internet family */
    ServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    ServAddr.sin_port = htons(ServPort);      /* Port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) < 0)
        DieWithSystemMessage("bind() failed");

    

    /* Go off and do real work */
    
	clntLen = sizeof(ClntAddr);
	
	short seqnum = 0;
	for(;;){ //server up forever
		//Recieve timrequests
		while((numbytes = recvfrom(sock, &request, sizeof(request), 0, (struct sockaddr *) &ClntAddr, &clntLen)) < 0);
		
		if(numbytes != sizeof(request)) //check what i have recieved
			DieWithSystemMessage("recvfrom() failed");
		
		//printf("Handling client %s\n", inet_ntoa(ClntAddr.sin_addr));
		response.seq = request.seq;
		response.version = request.version;
		response.client_nanoseconds =request.nanoseconds;
		response.client_seconds =request.seconds;
				
		//Get server timestamp
		int clock =  clock_gettime(CLOCK_REALTIME,time);
		if(clock<0)
			DieWithSystemMessage("clock() failed");
		response.server_nanoseconds=htobe64(time->tv_nsec);
		response.server_seconds=htobe64(time->tv_sec);
		
		
		//calculate probabily of dropping
		
		int prob = (rand() % (100- 1 + 1)) + 1;
		
		//send timeresponses only if we are in the range of numbers 100-drop
		
		if(prob>args.drop){
			
			//check seqnumber
			seqnum = ntohs(request.seq);
			
			if(seqnum>max_seqnum || seqnum == max_seqnum) //if sequence number is the highest seen, update it
				max_seqnum=seqnum;
		
			else{                  //if sequence number is lower than one seen before print it ouy
				printf("%s: %d %d %d\n",inet_ntoa(ClntAddr.sin_addr),ntohs(ClntAddr.sin_port),seqnum,max_seqnum);
				alarm(120);
			}	
			
			if (sendto(sock, &response, sizeof(response), 0, (struct sockaddr *)   &ClntAddr, sizeof(ClntAddr)) != sizeof(response))
				DieWithSystemMessage("sendto() failed");
		}
		
    }
       
}
void CatchAlarm(int ignored)     /* Handler for SIGALRM */
{	
	puts("alarm");
    max_seqnum=1;
    alarm(0);
}
