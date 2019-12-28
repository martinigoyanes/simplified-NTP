all:
	gcc	-Wall	-g	-o	client	src/client.c	src/diewithmessage.c	src/optparser.c
	gcc	-Wall	-g	-o	server	src/server.c	src/diewithmessage.c	src/optparser.c
