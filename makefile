CC = gcc
CFLAGS=-Wall

all: UDPChatClient UDPChatServer

OTHER_OBSS = DieWithError.o

UDPChatClient: UDPChatClient.o $(OTHER_OBSS)

UDPChatServer: UDPChatServer.o $(OTHER_OBSS)

DieWithError.o: DieWithError.c
	gcc -c DieWithError.c

UDPChatClient.o: UDPChatClient.c
	gcc -c UDPChatClient.c

UDPChatServer.o: UDPChatServer.c
	gcc -c UDPChatServer.c

clean:
	rm -f UDPChatClient.o UDPChatServer.o DieWithError.o UDPChatClient UDPChatServer


