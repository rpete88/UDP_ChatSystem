/* UDPChatServer- UDP Chat Program
 * author: Ryan Peterson
 *
 *
 */

#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MSGMAX 255	/* Longest string to send in a message */
#define USERMAX 32	/* Longest string to use as username */

void DieWithError(char *errorMessage);	/* Error handling function */
void forwardFile(char *fileName, int sock, (struct sockaddr *) clntAddr1, (struct sockaddr *) clntAddr2); /* file forwarding function */

int main(int argc, char *argv[])
{
	int sock;	/* Socket */
	struct sockaddr_in chatServAddr; /* Local address*/
	struct sockaddr_in fromAddr; /* Address of a received packet*/
	struct sockaddr_in chatClntAddr0; /* Client 0 address */
	struct sockaddr_in chatClntAddr1; /* Client 1 address */
	struct sockaddr_in chatClntAddr2; /* Client 2 address */
	unsigned short chatServPort; /* Server Port */
	unsigned int cliAddrLen; /* Length of incoming message */
	char chatBuffer[USERMAX+MSGMAX+1]; /* Buffer for chat string */
	char sendBuffer[USERMAX+MSGMAX+2]; /* +2 to include newline character and null termination */
	char usernameResp[3*(USERMAX +1)]; /*response to a new user */
	char *senderUsername; /*pointer to username of the current sender */
	char *chatMessage; /* pointer to the current chat server must transmit */
	int recvMsgSize; /* Size of the received message */
	char* usernames[3]; //[USERMAX+1];	/*array pointers to all usernames*/
	char USERNAMES[3][USERMAX+1];		/*array of usernames */
	int dontSend0, dontSend1, dontSend2;
	char *fileName;

	if ( argc != 2 ) /* test for the correct number of arguments */
	{
		fprintf(stderr, "Usage: %s <Server IP> \n", argv[0]);
		exit(1);
	}

	chatServPort = atoi(argv[1]); /* first argument: local port */

	/* Create Socket for incoming connections */
	if ( (sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP) ) <0)
		DieWithError( "socket() failed" );

	/* Construct local address structure */
	memset(&chatServAddr, 0, sizeof(chatServAddr)); /* Zero out structure */
	chatServAddr.sin_family = AF_INET; /* Internet address family */
	chatServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
	chatServAddr.sin_port = htons(chatServPort); /* Local Port */
	
	memset(&chatClntAddr0, 0, sizeof(chatClntAddr0)); /* Zero out structure */
	memset(&chatClntAddr1, 0, sizeof(chatClntAddr1)); /* Zero out structure */
	memset(&chatClntAddr2, 0, sizeof(chatClntAddr2)); /* Zero out structure */
	chatClntAddr0.sin_family = AF_INET; /*Internet address family */
	chatClntAddr1.sin_family = AF_INET; 
	chatClntAddr2.sin_family = AF_INET;

	/* Bind to the local address */
	if ( bind(sock, (struct sockaddr *)&chatServAddr, sizeof(chatServAddr)) <0)
		DieWithError( "bind() failed" );
	printf("Chatroom started\n");

	/*Initial pointers for usernames should be NULL */
	usernames[0] = NULL;
	usernames[1] = NULL;
	usernames[2] = NULL;
	for (;;) /* Run Forever */
	{
		/* Set the size of the in-out parameter */
		cliAddrLen = sizeof(fromAddr);
		memset(chatBuffer, '\0', sizeof(chatBuffer));
		/* Block until receive message from a client */
		/* messages from client follow format:
		 *		<message> 			- if message is meant for everyone in chat
		 *		<username> <message> 		- if message is meant for one user
		 *		<username>			- if client is attempting to enter chatroom
		 */
		/* messages from server follow format:
		 *		<sender>\n<message>		- if server is forwarding a message from a client
		 *		<username>\n <usernameList>	- if server is allowing user into chatroom (usernameList is separated with /n)
		 *		<newUser>\n "has entered the chat"	- if server is informing other users of a new member
		 */
		/* Invalid receive */
		if ( (recvMsgSize = recvfrom(sock, chatBuffer, USERMAX+MSGMAX+1, 0, (struct sockaddr *)&fromAddr, &cliAddrLen)) < 0)
			DieWithError( "recvfrom() failed" );
		/* we have received a message from a valid user */
		else if ( (fromAddr.sin_port == chatClntAddr0.sin_port) ||
				(fromAddr.sin_port == chatClntAddr1.sin_port) ||
			       	(fromAddr.sin_port == chatClntAddr2.sin_port) ) 
		{
			/* find what user message comes from */
			if( fromAddr.sin_port== chatClntAddr0.sin_port )
				senderUsername = usernames[0];
			else if( fromAddr.sin_port == chatClntAddr1.sin_port )
				senderUsername = usernames[1];
			else if( fromAddr.sin_port == chatClntAddr2.sin_port )
				senderUsername = usernames[2];
			else /*Should not get to here */
				DieWithError("ERROR IN USERNAME ASSIGNMENT CODE");
			/* Check if message is a private chat */
			chatBuffer[recvMsgSize] = '\0';
			printf("Received from %s\n", senderUsername);
			/* begin constructing message */
			memset(sendBuffer, '\0', sizeof(sendBuffer)); /* zero out structure to send back messages */
			strcpy(sendBuffer, senderUsername); /* place username of sender at the beginning of message */
			strcat(sendBuffer, "\n"); /* add a newline character */
			chatMessage = strtok(chatBuffer, " "); /* A pointer to the first part of the message */
			if( !strcmp(chatMesssage, "fileTransfer") ) { /*check if chatMessage is a fileTransfer */
				fileName = strtok(NULL, " ");
				if( usernames[0] && !strcmp(senderUsername, usernames[0]) ) 
				{
					/* from clnt 0 */
					if( usernames[1] )
						if( sendto(sock, chatBuffer, sizeof(chatBuffer), 0, (struct sockaddr *)chatClntAddr1, sizeof(chatClntAddr1) )!= sizeof(chatBuffer) )
							DieWithError("failed to forward file fileTransfer message");
					if( usernames[2] )
						if( sendto(sock, chatBuffer, sizeof(chatBuffer), 0, (struct sockaddr *)chatClntAddr2, sizeof(chatClntAddr2) )!= sizeof(chatBuffer) )
							DieWithError("failed to forward file fileTransfer message");
					forwardFile(fileName, sock, chatClntAddr1, chatClntAddr2);
				}
				else if( usernames[1] && !strcmp(sendUsername, usernames[1]) ) 
				{
					/* from clnt 1 */
					if( usernames[0] )
						if( sendto(sock, chatBuffer, sizeof(chatBuffer), 0, (struct sockaddr *)chatClntAddr0, sizeof(chatClntAddr0) )!= sizeof(chatBuffer) )
							DieWithError("failed to forward file fileTransfer message");
					if( usernames[2] )
						if( sendto(sock, chatBuffer, sizeof(chatBuffer), 0, (struct sockaddr *)chatClntAddr2, sizeof(chatClntAddr2) )!= sizeof(chatBuffer) )
							DieWithError("failed to forward file fileTransfer message");
					forwardFile(fileName, sock, chatClntAddr0, chatClntAddr2);
				}
				else if ( usernames[2] && !strcmp(senderUsername, usernames[2]) ) 
				{
					/* from clnt 2 */
					if( usernames[0] )
						if( sendto(sock, chatBuffer, sizeof(chatBuffer), 0, (struct sockaddr *)chatClntAddr0, sizeof(chatClntAddr0) )!= sizeof(chatBuffer) )
							DieWithError("failed to forward file fileTransfer message");
					if( usernames[1] )
						if( sendto(sock, chatBuffer, sizeof(chatBuffer), 0, (struct sockaddr *)chatClntAddr1, sizeof(chatClntAddr1) )!= sizeof(chatBuffer) )
							DieWithError("failed to forward file fileTransfer message");
					forwardFile(fileName, sock, chatClntAddr0, chatClntAddr1);
				}	
			}
			else {
				/* If the first part of the message is a username, we only want to send to that user */
				if( usernames[0] && !strcmp(chatMessage, usernames[0]) ) /* only want to send to usernames[0] */
				{
					printf("Detected private message for user0\n");
					dontSend0 = 0;
					dontSend1 = 1;
					dontSend2 = 1;
				}
				else if( usernames[1] && !strcmp(chatMessage, usernames[1]) ) /* only want to send to usernames[1] */
				{
					printf("Detected private message for user1\n");
					dontSend0 = 1;
					dontSend1 = 0;
					dontSend2 = 1;
				}
				else if( usernames[2] && !strcmp(chatMessage, usernames[2]) ) /* only want to send to usernames[2] */
				{
					printf("Detected private message for user2\n");
					dontSend0 = 1;
					dontSend1 = 1;
					dontSend2 = 0;
				}
				else  /*first part of message is not a username, send to all*/
				{
					dontSend0 = 0;
					dontSend1 = 0;
					dontSend2 = 0;
					strcat(sendBuffer, chatMessage); /* the first part of the message is not a username */
					strcat(sendBuffer, " ");	 /* so we add that back into message with a space */
				}
				chatMessage = strtok(NULL, ""); /* grab the rest of the message*/
				if (chatMessage) {
					strcat(sendBuffer, chatMessage); /* place message in send buffer */
				}
				sendBuffer[USERMAX+MSGMAX+2] = '\0';
				printf("Sending message '%s' to client(s)....\n", sendBuffer);
				/* Send Message */
				if ( usernames[0] && !strcmp(senderUsername, usernames[0]) ) /*message from first user*/
				{
					if(usernames[1] && !dontSend1) /* Send to second user if valid */
					{
						if( sendto(sock, sendBuffer, USERMAX+MSGMAX+1, 0, (struct sockaddr *)&chatClntAddr1, sizeof(chatClntAddr1)) <0)
							DieWithError("sendto() failed for usernames[0] to usernames[1].");
						printf("     Sent to %s\n", usernames[1]);
					}
					if(usernames[2] && !dontSend2) /* Send to third user if valid */
					{
						if( sendto(sock, sendBuffer, USERMAX+MSGMAX+1, 0, (struct sockaddr *)&chatClntAddr2, sizeof(chatClntAddr2)) <0)
							DieWithError("sendto() failed for usernames[0] to usernames[2].");
						printf("    Sent to %s\n", usernames[2]);
	
					}
				}
				else if( usernames[1] && !strcmp(senderUsername, usernames[1] )) /* message from second user */
				{
					if(usernames[0] && !dontSend0) /* Send to second user if valid */
					{
						if( sendto(sock, sendBuffer, USERMAX+MSGMAX+1, 0, (struct sockaddr *)&chatClntAddr0, sizeof(chatClntAddr0)) <0)
							DieWithError("sendto() failed for usernames[1] to usernames[0].");
						printf("    Sent to %s\n", usernames[0]);

					}
					if(usernames[2] && !dontSend2) /* Send to third user if valid */
					{
						if( sendto(sock, sendBuffer, USERMAX+MSGMAX+1, 0, (struct sockaddr *)&chatClntAddr2, sizeof(chatClntAddr2)) <0)
							DieWithError("sendto() failed for usernames[1] to usernames[2].");
						printf("    Sent to %s\n", usernames[2]);

					}
				}
				else if( usernames[2] && !strcmp(senderUsername, usernames[2] )) /* message from third user */
				{
					if(usernames[0] && !dontSend0) /* Send to second user if valid */
					{
						if( sendto(sock, sendBuffer, USERMAX+MSGMAX+1, 0, (struct sockaddr *)&chatClntAddr0, sizeof(chatClntAddr0)) <0)
							DieWithError("sendto() failed for usernames[2] to usernames[0].");
						printf("    Sent to %s\n", usernames[0]);
	
					}
					if(usernames[1] && !dontSend1) /* Send to third user if valid */
					{
						if( sendto(sock, sendBuffer, USERMAX+MSGMAX+1, 0, (struct sockaddr *)&chatClntAddr1, sizeof(chatClntAddr1)) <0)
							DieWithError("sendto() failed for usernames[2] to usernames[1].");
						printf("    Sent to %s\n", usernames[1]);
	
					}
				}
				else /* There is nobody to send the message to, server can discard */
				{
					printf("    Nobody to send to.\n");
				}
			}

		}
		/* A user has attempted to enter chat */
		else if ( !usernames[0] || !usernames[1] || !usernames[2] )
		{
			/*Construct new addition messages*/
			memset(sendBuffer, '\0', sizeof(sendBuffer)); /* zero out structures */
			memset(usernameResp, '\0', sizeof(usernameResp));
			strcpy(sendBuffer, chatBuffer); /* copy new username to messages */
			strcpy(usernameResp, chatBuffer);
			strcat(sendBuffer, "\nhas entered the chat"); /* add message content */
			/*where can we add client */
			if ( !usernames[0] ) /* first username slot is open */ 
			{
				memset(USERNAMES[0], '\0', USERMAX); /*zero out structure*/
				strcpy(USERNAMES[0], chatBuffer); /*put in first spot */
				chatClntAddr0 = fromAddr; /*add address*/
				printf("added %s- address:%d, port:%d\n", USERNAMES[0], chatClntAddr0.sin_addr.s_addr, chatClntAddr0.sin_port);
				usernames[0] = USERNAMES[0]; /*point to username*/
				/* Send a response to client */
				if( sendto(sock, usernameResp, 3*(USERMAX+1), 0, (struct sockaddr *)&chatClntAddr0, sizeof(chatClntAddr0)) <0 )
					DieWithError( "sendto() failed to respond to new client" );
			}
			else if ( !usernames[1] ) /* second username slot is open */
			{
				memset(USERNAMES[1], '\0', USERMAX); /* Zero out structure */
				strcpy(USERNAMES[1], chatBuffer); /* put in second spot */
				chatClntAddr1 = fromAddr; /* add address */
				printf("added %s- address:%d, port:%d\n", USERNAMES[1],chatClntAddr1.sin_addr.s_addr, chatClntAddr1.sin_port);
				usernames[1] = USERNAMES[1]; /*point to username*/
				/* Send reponse to client */
				strcat(usernameResp, "\n");
				strcat(usernameResp, USERNAMES[0]);
				printf("--usernameResp: %s\n", usernameResp);
				printf("--sendBuffer: %s\n", sendBuffer);
				if( sendto(sock, usernameResp, 3*(USERMAX+1), 0, (struct sockaddr *)&chatClntAddr1, sizeof(chatClntAddr1)) <0 )
					DieWithError( "sendto() failed to respond to new client" );
				/* inform other user a new user has entered chatroom */
				/* THIS SENDTO() IS NOT BEING RECEIVED BY CLIENT */
				if( sendto(sock, sendBuffer, USERMAX+22, 0, (struct sockaddr *)&chatClntAddr0, sizeof(chatClntAddr0)) <0 )
					DieWithError( "sendto() failed to awknowledge new client" );
				printf("sent message to %s that new client has entered chatroom.\n", usernames[0]);
			}
			else if ( !usernames[2] ) /* third username slot is open */
			{	
				memset(USERNAMES[2], '\0', USERMAX); /*Zero out structure */
				strcpy(USERNAMES[2], chatBuffer); /* put in third spot */
				chatClntAddr2 = fromAddr; /* add address */
				printf("added %s- address:%d, port:%d\n", USERNAMES[2],chatClntAddr2.sin_addr.s_addr, chatClntAddr2.sin_port);
				usernames[2] = USERNAMES[2]; /* point to username */
				/* Send response to client */
				strcat(usernameResp, "\n");
				strcat(usernameResp, USERNAMES[0]);
				strcat(usernameResp, "\n");
				strcat(usernameResp, USERNAMES[1]);
				if( sendto(sock, usernameResp, 3*(USERMAX+1), 0, (struct sockaddr *)&chatClntAddr2, sizeof(chatClntAddr2)) <0 )
					DieWithError( "sendto() failed to respond to new client" );
				/* inform other users a new user has entered chatroom*/
				if( sendto(sock, sendBuffer, USERMAX+21, 0, (struct sockaddr *)&chatClntAddr0, sizeof(chatClntAddr0)) <0 )
					DieWithError( "sendto() failed to awknowledge new client" );
				if( sendto(sock, sendBuffer, USERMAX+21, 0, (struct sockaddr *)&chatClntAddr1, sizeof(chatClntAddr1)) <0 )
					DieWithError( "sendto() failed to awknowledge new client" );
			}
			else
			{
				DieWithError("ERROR IN CHAT ENTRANCE CODE");
			}
		}
		/* we have a full chatroom */
		else
		{
			printf("Chatroom is full. %s, %s, %s.\n", usernames[0], usernames[1], usernames[2]);
			if(sendto(sock, "Chatroom is full", 20, 0, (struct sockaddr *)&fromAddr, sizeof(fromAddr)) <0 )
				DieWithError("sendto() for full chat did not work as expected");
		}
	}
	/* NOT REACHED */ 
}
