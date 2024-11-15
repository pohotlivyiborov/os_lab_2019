CC = gcc
CFLAGS = -I.

TASK4_DIR = /workspaces/os_lab_2019/lab4/src
TASK3_DIR = /workspaces/os_lab_2019/lab3/src

all: process_memory parallel_min_max

process_memory: $(TASK4_DIR)/process_memory.c
	$(CC) $(CFLAGS) -o process_memory $(TASK4_DIR)/process_memory.c 

parallel_min_max: $(TASK3_DIR)/utils.o $(TASK3_DIR)/find_min_max.o $(TASK3_DIR)/parallel_min_max.c
	$(CC) $(CFLAGS) -o parallel_min_max $(TASK3_DIR)/utils.o $(TASK3_DIR)/find_min_max.o $(TASK3_DIR)/parallel_min_max.c 

$(TASK3_DIR)/utils.o: $(TASK3_DIR)/utils.c $(TASK3_DIR)/utils.h
	$(CC) $(CFLAGS) -c $(TASK3_DIR)/utils.c -o $(TASK3_DIR)/utils.o

$(TASK3_DIR)/find_min_max.o: $(TASK3_DIR)/find_min_max.c $(TASK3_DIR)/find_min_max.h
	$(CC) $(CFLAGS) -c $(TASK3_DIR)/find_min_max.c -o $(TASK3_DIR)/find_min_max.o

clean:
	rm -f process_memory parallel_min_max $(TASK3_DIR)/*.o