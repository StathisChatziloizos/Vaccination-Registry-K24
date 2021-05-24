program: travelMonitor.c Monitor.c bloom.c libTM.c hash.c bst.c skip_list.c

	gcc -g -o travelMonitor travelMonitor.c bloom.c libTM.c
	gcc -g -o Monitor Monitor.c hash.c bst.c bloom.c skip_list.c


clean:
	rm -rf program child signal asigact travelMonitor Monitor

pipes:
	rm -rf fifo*

log:
	rm -rf log_file*