.PHONY:clean

all:SE_server

SE_server:SE_server.c
	gcc SE_server.c -o SE_server -lpthread -lwebsockets

example-server:example-server.c
	gcc example-server.c -o example-server -lwebsockets

clean:
	-rm SE_sever example-server