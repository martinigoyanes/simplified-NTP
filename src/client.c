#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <endian.h>
#include "Practical.h"
#include "optparser.h"
#include "assigment1.h"
#include "math.h"


void DieWithError(char *errorMessage);   /* Error handling function */
void CatchAlarm(int ignored);            /* Handler for SIGALRM */
void calculations (struct timeresponse response , double *theta, double *delta){
		
		double nano,secs,t0,t1,t2;
		struct timespec *time = malloc(sizeof(struct timespec));
		
		//Get current time (T2)
		
		int clock =  clock_gettime(CLOCK_REALTIME,time);
		if(clock<0)
			DieWithSystemMessage("clock() failed");
			
		nano=(double)time->tv_nsec;
		nano = (nano)/(pow(10,9));
		
		secs =(double)time->tv_sec;
		
		t2 = secs+nano;
		
		
		
		//Get client stamp (T0)
		
		nano = (double)response.client_nanoseconds;
		nano = (nano)/(pow(10,9));
		
		secs = (double)response.client_seconds;
		
		t0 = secs+nano;
		
		
		//Get server stamp (T1)
		
		nano = (double)response.server_nanoseconds;
		nano = (nano)/(pow(10,9));
		
		secs = (double)response.server_seconds;
		
		
		t1 = secs+nano;
		
		
		*theta = ((t1-t0)+(t1-t2))/2;
		*delta = t2-t0;
		
}

int main(int argc, char *argv[]) {
  struct client_arguments args = client_parseopt(argc, argv); 
  struct timespec *time = malloc(sizeof(struct timespec));
  struct timerequest request;
  struct timeresponse response;
  struct clientoutput output [args.reqnum];
  struct sigaction myAction;       /* For setting signal handler */
  int n=1; 
  double *delta = malloc(sizeof(double));
  double *theta = malloc(sizeof(double));
  
  // Tell the system what kind(s) of address info we want
  struct addrinfo addrCriteria;                   // Criteria for address
  memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
  addrCriteria.ai_family = AF_UNSPEC;             // Any address family
  addrCriteria.ai_socktype = SOCK_DGRAM;          // Only datagram sockets
  addrCriteria.ai_protocol = IPPROTO_UDP;         // Only UDP protocol
   
  
  // Get address(es)
  char *portstring = malloc((char)10);
  sprintf(portstring,"%d",args.port);
  struct addrinfo *servAddr; // Holder for returned list of server addrs
  int rtnVal = getaddrinfo(args.ip_address, portstring, &addrCriteria, &servAddr);
  if (rtnVal != 0)
    DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));

  // Create a reliable, stream socket using UDP
  int sock = socket(servAddr->ai_family, servAddr->ai_socktype,
      servAddr->ai_protocol); // Socket descriptor for client
  if (sock < 0)
    DieWithSystemMessage("socket() failed");

 
  
  ssize_t numbytes;
  //~ int a=0;
  while(n<args.reqnum+1){
  // SETUP PACKET
  //~ if(n==6 && a==0){
	//~ n=4;
	//~ a=1;
  //~ }
  
  request.seq=htons(n);
  request.version=htons(3);
  int clock =  clock_gettime(CLOCK_REALTIME,time);
  if(clock<0)
	  DieWithSystemMessage("clock() failed");
  request.nanoseconds=htobe64(time->tv_nsec);
  request.seconds=htobe64(time->tv_sec);
  
  //SEND
  
  
  numbytes = sendto(sock, &request, sizeof(request), 0, servAddr->ai_addr, servAddr->ai_addrlen);
  if (numbytes < 0)
    DieWithSystemMessage("sendto() failed");
  else if (numbytes != sizeof(request))
    DieWithUserMessage("sendto() error", "sent unexpected number of bytes");
  n++;
  
  }	
  // Receive a response

  struct sockaddr_storage fromAddr; // Source address of server
  
  // Set length of from address structure (in-out parameter)
  socklen_t fromAddrLen = sizeof(fromAddr);
  
  /* Set signal handler for alarm signal */
    myAction.sa_handler = CatchAlarm;
    if (sigfillset(&myAction.sa_mask) < 0) /* block everything in handler */
        DieWithError("sigfillset() failed");
    myAction.sa_flags = 0;

    if (sigaction(SIGALRM, &myAction, 0) < 0)
        DieWithError("sigaction() failed for SIGALRM");
  
  n=0;
  ////initialize output to 0
  
  for(int i = 1; i < args.reqnum+1 ; i++){
		output[i].seq = 0;
	}
  
  while(n<args.reqnum){
	  
	  alarm(args.timeout); // Set the timeout
	  
	  while ((numbytes = recvfrom(sock, &response,sizeof(response), 0, (struct sockaddr *) &fromAddr, &fromAddrLen))<0){
		  if (errno == EINTR){
			for(int i = 1 ; i<args.reqnum+1; i++){
				if(output[i].seq == 0)
					printf("%d: Dropped\n",i);
				else
					printf("%d: %f %f\n",i,output[i].theta,output[i].delta);
			}
			exit(0);
		}
	  }
	 
	 // recvfrom() got something -- cancel the timeout
	 alarm(0);
	  
	 //transform to little endian 
	  response.seq = ntohs(response.seq);
	  response.version = ntohs(response.version);
	  response.client_nanoseconds = be64toh(response.client_nanoseconds);
	  response.client_seconds = be64toh(response.client_seconds);
	  response.server_nanoseconds = be64toh(response.server_nanoseconds);
	  response.server_seconds = be64toh(response.server_seconds);
		
	  //obtain theta and delta
	  calculations(response,theta,delta);
	  
	  //fill out the timerequests that actually got an answer
	  output[response.seq].seq = response.seq;
	  output[response.seq].theta = *theta;
	  output[response.seq].delta = *delta;
	  
	  //increment number of time request received
	  n++;
  }	
  alarm(0);
  
  for(int i = 1 ; i<args.reqnum+1; i++){
	  if(output[i].seq == 0)
		printf("%d: Dropped\n",i);
	  else
	   printf("%d: %f %f\n",i,output[i].theta,output[i].delta);
  }
  
  close(sock);
  exit(0);
}
void CatchAlarm(int ignored)     /* Handler for SIGALRM */
{
    
}
void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

