program: travelMonitorClient.c monitorServer.c threadsMonitorServer.c bloom.c libTM.c hash.c bst.c skip_list.c

	gcc -g -o travelMonitorClient travelMonitorClient.c bloom.c libTM.c
	gcc -g -o monitorServer monitorServer.c hash.c bst.c bloom.c skip_list.c
	gcc -g -o threadsMonitorServer threadsMonitorServer.c hash.c bst.c bloom.c skip_list.c -lpthread


clean:
	rm -rf travelMonitorClient monitorServer threadsMonitorServer

log:
	rm -rf log_file*