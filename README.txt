Client-Server Chat System
author: Ryan Peterson
class: CISC450/CPEG419

Protocol Design:
	A protocol must be followed in order for successfully communication to be accomplished.
	All messages from server to client-
		<senderUsername>\n<message>        if server is forwarding a message from senderUsername
		<username>\n<usernameList>	   if server is accepting username into the chatroom
	All messages from client to server-
		<message>			   if client is sending message to everyone in chatroom
		<privateUsername>\n<message>	   if client is sending a private message to privateUsername
	        <username>			   this is first message from client, for permission to enter chatroom

List of relevant files:
	UDPChatClient.c
	UDPChatServer.c
	DieWithError.c
	makefile

Compilation Instructions:
	Use the following commands to generate executables for chat system-
		gcc -c UDPChatClient.c
		gcc -c UDPChatServer.c
		gcc -c DieWithError.c
		gcc -o UDPChatClient UDPChatClient.o DieWithError.o
		gcc -o UDPChatServer UDPChatServer.o DieWithError.o

		There is a makefile for this program with the commands:
			make
			make clean

Running Instructions:
	There are two executables generated: UDPChatServer and UDPChatClient.
	- The function of UDPChatServer is to create a socket, bind socket to local address, listen for incoming connections, and handle incoming clients by transferring messages sent to it
	- The function of UDPChatClient is to create a socket, connect to transfer server, and send/receive messages to/from server.
	- In order to run UDPChatServer, a port number must be specified. Use command:
		./UDPChatServer <port number>

	- In order to run TCPFileTransferClient, a host IP, username, and server port number must be specified. Use command:
		./UDPChatClient <IP address> <username> <port number>
		note- IP address of 127.0.0.1 can be used
