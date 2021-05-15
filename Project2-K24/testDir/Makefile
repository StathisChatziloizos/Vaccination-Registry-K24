program: parent.c child.c signal.c asigact.c travelMonitor.c Monitor.c
	gcc -g -o program parent.c
	gcc -g -o child child.c
	gcc -g -o signal signal.c
	gcc -g -o asigact asigact.c
	gcc -g -o travelMonitor travelMonitor.c bloom.c -lm
	gcc -g -o Monitor Monitor.c hash.c bst.c bloom.c skip_list.c





child: child.c
	gcc -Wall -o child child.c
clean:
	rm -rf program child signal asigact travelMonitor Monitor

pipes:
	rm -rf fifo*