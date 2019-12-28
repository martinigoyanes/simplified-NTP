#include <stdint.h>
struct timerequest {
   uint16_t seq; 
   uint16_t version;
   uint64_t nanoseconds;
   uint64_t seconds;
   
} __attribute__((packed));
struct timeresponse {
   uint16_t seq; 
   uint16_t version;
   uint64_t client_nanoseconds;
   uint64_t client_seconds;
   uint64_t server_nanoseconds;
   uint64_t server_seconds;
} __attribute__((packed));
struct clientoutput{
	int seq;
	double theta;
	double delta;	
};
