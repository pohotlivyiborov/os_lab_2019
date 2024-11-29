CC = gcc
CFLAGS = -pthread

SERVER_SRC = server.c
CLIENT_SRC = client.c
COMMON_SRC = common.c

SERVER_OBJ = $(SERVER_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)
COMMON_OBJ = $(COMMON_SRC:.c=.o)

SERVER_EXE = server
CLIENT_EXE = client

LIB_NAME = libcommon.a

all: $(SERVER_EXE) $(CLIENT_EXE)

$(SERVER_EXE): $(SERVER_OBJ) $(LIB_NAME)
	$(CC) $(CFLAGS) -o $@ $(SERVER_OBJ) -L. -lcommon

$(CLIENT_EXE): $(CLIENT_OBJ) $(LIB_NAME)
	$(CC) $(CFLAGS) -o $@ $(CLIENT_OBJ) -L. -lcommon

$(LIB_NAME): $(COMMON_OBJ)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(SERVER_OBJ) $(CLIENT_OBJ) $(COMMON_OBJ) $(SERVER_EXE) $(CLIENT_EXE) $(LIB_NAME)

rebuild: clean all

run_server: $(SERVER_EXE)
	./$(SERVER_EXE) --port 20001 --tnum 4

run_client: $(CLIENT_EXE)
	./$(CLIENT_EXE) --k 4 --mod 7 --servers servers.txt

.PHONY: all clean rebuild run_server run_client