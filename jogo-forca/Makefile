all:
	gcc -Wall libs/common.c -c
	gcc -Wall libs/socketHandler.c -c
	gcc -Wall common.o socketHandler.o client.c -o client 
	gcc -Wall common.o socketHandler.o server.c -o server

clean:
	rm -f client server common.o socketHandler.o
