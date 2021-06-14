socket: socket.c client.c par.c chi.c threads.c
	gcc -g -o socket socket.c
	gcc -g -o client client.c
	gcc -g -o par par.c
	gcc -g -o threads threads.c -lpthread
	gcc -g -o chi chi.c

clean:
	rm -rf socket client chi par threads