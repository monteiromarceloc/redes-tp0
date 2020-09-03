all:
	gcc -Wall common.c -c
	gcc -Wall common.o client.c -o client 
	gcc -Wall common.o server.c -o server

clean:
	rm -f client server common.o
