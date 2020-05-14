/* UDPEchoClient- UDP Chat Program
 * author: Ryan Peterson
 *
 * Contains code necessary for a client to create a socket,
 * connect with a server, and send/receive messages from
 * other clients connected to the server. The DieWithError(1)
 * error handling function is used to exit program if problems 
 * arise with the UDP functions.
 */

#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send() and recv() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <stdlib.h>
#include <string.h>
#include <sys/time.h> /* used for select timeval structure */
#include <sys/select.h> /*select function for synchronous I/O multiplexing */
#include <sys/types.h> /*for FD_SET to read file descriptors */
#include <unistd.h>

#define MSGMAX 255	/* Longest message to send */
#define USERMAX 32	/* Longest username to send */

void DieWithError(char *errorMessage); /*Error Handling function */

int main(int argc, char *argv[])
{
	int sock; /*Socket descriptor */
	struct sockaddr_in chatServAddr; /* Chat Server Address */
	struct sockaddr_in fromAddr; /* Source Address of message */
	unsigned short chatServPort;	/* Chat Server Port */
	unsigned int chatServSize;
	unsigned int fromSize;
	char *servIP;	/*Server IP address*/
	char *senderUsername;	/* username of received message */
	char *recvChatMsg;		/* received chat message */
	char usernameBuffer[3*USERMAX +1]; /* Buffer to hold all usernames returned by server */
	char chatBuffer[USERMAX+ MSGMAX +1];	/* Buffer to hold message contents sent by server */
	char *username; /*initial username attempt*/
	char *usernames[3]; /*array of pointers to all usernames */
	char USERNAMES[3][USERMAX]; /*array of all usernames */
	int i; 	/* counter */
	unsigned int usernameStringLen;	/*length of username string */
	unsigned int chatStringLen;	/* length of chat message */
	int recvMsgLen;	/* Length of the message we receive from server */
	int respUsernameLen; /* Length of the username response we get from server */
	fd_set rfds; /* what to watch stdin to see when it has input */
	struct timeval timer; /* select function has time value */
	int retval; /*return value from the select function */

	/* Test for correct number of arguments */
	if ( (argc<3) || (argc>4) )
	{
		fprintf(stderr, "Usage: %s <Server IP> <Username> [<Chat Port>]\n"), argv[0];
		exit(1);
	}
	servIP = argv[1];
	username = argv[2];

	if ( (usernameStringLen = strlen(username)) > USERMAX) /* Check message length */
		DieWithError( "Username length too long" );

	if (argc == 4)
		chatServPort = atoi(argv[3]);
	else
		chatServPort = 7;

	/* Create a datagram/UDP socket */
	if ( (sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP) ) <0)
		DieWithError( "socket() failed" );
	/* Construct the server address structure */
	memset(&chatServAddr, 0, sizeof(chatServAddr)); /* Zero out structure */
	chatServAddr.sin_family = AF_INET; /* Internet addr family */
	chatServAddr.sin_addr.s_addr = inet_addr(servIP); /* Server IP address */
	chatServAddr.sin_port = htons(chatServPort); /*Server port */
	/* initialize username pointers to null */
	usernames[0] = NULL;
	usernames[1] = NULL;
	usernames[2] = NULL;
	/* Send the username to the server */
	printf("sending username to server.\n");
	if ( sendto(sock, username, usernameStringLen, 0, (struct sockaddr *)&chatServAddr, sizeof(chatServAddr)) != usernameStringLen)
		DieWithError( "send() sent a different number of bytes than expected");
	/* Receive a response */
	printf("waiting for response.\n");
	fromSize = sizeof(chatServAddr);
	if ((respUsernameLen = recvfrom(sock, usernameBuffer, 3*USERMAX+1, 0, (struct sockaddr *)&fromAddr, &fromSize)) < usernameStringLen)
	      DieWithError( "recvfrom() failed" );

	if (chatServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
	{
		printf("%d : %d\n", chatServAddr.sin_addr.s_addr, fromAddr.sin_addr.s_addr);
		fprintf(stderr, "Error: received a packet from unknown source.\n");
		exit(1);
	}
	/* null-terminate the received data */
	usernameBuffer[respUsernameLen] = '\0';
	i = 0;
	/* separate usernames from the server username response */
	username = strtok(usernameBuffer, "\n");
	while( username != NULL)
	{
		memset(USERNAMES[i], '\0', USERMAX);
		strcpy(USERNAMES[i], username);
		usernames[i] = USERNAMES[i];
		i++;
		if (i == 3)
			break;
		username = strtok(NULL, "\n");
	}
	i=0;
	printf("Your username is: %s\n", usernames[0]); /* print the client's assigned username */
	if( !usernames[1])
		printf("No one else is in chatroom.\n");
	else
	{
		printf("Others in chatroom: %s", usernames[1]);
		if( usernames[2] )
			printf(", %s\n", usernames[2]);
		else
			printf("\n");
	}
	printf("To send a private message, include the username you wish to send to at the beginning of the message.\n");
	printf("To send a file, include 'fileTransfer' at the beginning of the message.\n\n");
	
	/* clears set/ initialize a file descriptor set*/
	FD_ZERO(&rfds);
	/* Set wait time- zero valued to effect a poll */
	timer.tv_sec = 0; /*seconds*/
	timer.tv_usec = 0; /*microseconds*/

	/* Continuously send and receive messages from chatroom*/
	for(;;) 
	{
		/* messages from client follow format:
		 *		<message> 			- if message is meant for everyone in chat
		 *		<username> <message> 		- if message is meant for one user
		 *		<username>			- if client is attempting to enter chatroom
		 */
		/* messages from server follow format:
		 *		<sender>\n<message>		- if server is forwarding a message from a client
		 *		<username>\n<usernameList>	- if server is allowing user into chatroom (usernameList is separated with /n)
		 *		<newUser>\n"has entered the chat"	- if server is informing other users of a new member
		 */

		FD_SET(0, &rfds); /* watch stdin (fd 0) to see when it has input */
		FD_SET(sock, &rfds); /* watch for socket */
		retval = select(sock+1, &rfds, NULL, NULL, &timer);
		/* Sending a message */
		if( retval == -1) /* we have a message to send */
		{
			DieWithError("select() failed");
		}
		else if (retval) {
			memset( chatBuffer, '\0', sizeof(chatBuffer) );
			/* After select returns, readfds will be cleared except for file descriptors ready for reading */
			if( FD_ISSET(0, &rfds) ) { /*we have a message to send to server*/
				/*get input from user*/
				read(0, chatBuffer, USERMAX+MSGMAX+1);
				chatBuffer[USERMAX+MSGMAX+1] = '\0'; /* Null terminate message */
				chatStringLen = strlen(chatBuffer);
				chatBuffer[chatStringLen] ='\0';
				/*send to server*/
				if( sendto(sock, chatBuffer, chatStringLen, 0, (struct sockaddr *)&chatServAddr, sizeof(chatServAddr))!= chatStringLen)
					DieWithError("sendto() sent a different number of bytes than expected");
				printf("%s: %s\n", usernames[0], chatBuffer);
			}
			else if ( FD_ISSET(sock, &rfds) ) { /* we have received message from server */
				if( (recvMsgLen = recvfrom(sock, chatBuffer, USERMAX+MSGMAX+1, 0, (struct sockaddr *)&fromAddr, &fromSize)) <0)
					DieWithError( "recvfrom() failed");
				else if( chatServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr && recvMsgLen > 0)
				{
					fprintf(stderr, "Error: received a packet from unknown source.\n");
					exit(1);
				}
				else
				{
					/* Null terminate the received data */
					chatBuffer[USERMAX+MSGMAX+1] = '\0';
					/* separate username of sender */
					senderUsername = strtok(chatBuffer, "\n");
					recvChatMsg = strtok(NULL, "\n"); 
					/* print username of sender and received chat message */
					printf("%s: %s\n", senderUsername, recvChatMsg);
				}
			}
		}
		FD_ZERO(&rfds);
	}
	close(sock);
	exit(0);
}
