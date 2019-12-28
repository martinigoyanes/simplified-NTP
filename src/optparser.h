#include <argp.h>
struct server_arguments {
	int port;
	int drop;
};
struct client_arguments {
	char ip_address[16]; /* You can store this as a string, but I probably wouldn't */
	int port; /* is there already a structure you can store the address
	           * and port in instead of like this? */
	int reqnum;
	int timeout;
	
};
error_t server_parser(int key, char *arg, struct argp_state *state);
struct server_arguments server_parseopt(int argc, char *argv[]) ;
error_t client_parser(int key, char *arg, struct argp_state *state);
struct client_arguments client_parseopt(int argc, char *argv[]);
