# vaccineMonitor Makefile
# FILES: library.c  library.h  main.c  Makefile

OBJS 	= vaccineMonitor.o bloom.o bst.o hash.o skip_list.o
SOURCE 	= vaccineMonitor.c bloom.c bst.c hash.c skip_list.c
HEADER	= bloom.h bst.h hash.h skip_list.h
OUT	= vaccineMonitor
CC	= gcc
FLAGS	= -g

$(OUT):	$(OBJS)
	$(CC) $(FLAGS) -o $(OUT) $(OBJS)

vaccineMonitor.o:	vaccineMonitor.c
	$(CC) $(FLAGS) -c vaccineMonitor.c

bloom.o: bloom.c
	$(CC) $(FLAGS) -c bloom.c

bst.o: bst.c
	$(CC) $(FLAGS) -c bst.c

hash.o: hash.c
	$(CC) $(FLAGS) -c hash.c

skip_list.o: skip_list.c
	$(CC) $(FLAGS) -c skip_list.c

clean:
	rm -f $(OBJS) $(OUT)

count:
	wc $(SOURCE) $(HEADER)