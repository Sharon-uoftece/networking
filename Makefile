all: 
	gcc -c -pthread server.c
	gcc -c -pthread client.c
	gcc -c -pthread client_utils.c
	gcc -c -pthread server_utils.c
	gcc -pthread client.o client_utils.o -o client
	gcc -pthread server.o server_utils.o -o server
	rm -f client.o
	rm -f server.o
	rm -f client_utils.o
	rm -f server_utils.o

clean:
	rm -f server client client_utils.o server_utils.o