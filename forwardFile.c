/* forwardFile.c- UDP Chay System
 * author: Ryan Peterson
 *
 * Contains code for a server to receive the contents of
 * a file from a client, and forward the message so each 
 * of the other clients.
 */

#include <stdio.h> /* for printf() and fprintf() */
#include <unistd.h> /* for close() */
#include <sys/socket.h>
#include <sys/inet.h>

#define FILEBUFSIZE 255 /* size of the file buffer */

void DieWithError(char *errorMessage);

void forwardFile(char *fileName, int sock, (struct sockaddr *) clntAddr1, (struct sockaddr *) clntAddr2 ) 
{
	struct sockaddr_in fromAddr;
	int bytesRecv;	
	int flag = 1;
	char fileBuffer[FILEBUFSIZE+1];
	char *endOfFile;

	while(flag) {
		memset(fileBuffer, '\0', sizeof(fileBuffer));
		/* Receive some file contents */
		if( (bytesRecv = recvfrom(senderSock, fileBuffer, sizeof(fileBuffer), 0, (struct sockaddr *)&fromAddr, sizeof(fromAddr)) ) <0 )
			DieWithError("Recvfrom() failed for file forwarding");
		/* last message will be 'close filename.ext' */
		endOfFile = strtok(fileBuffer, " ");
		if( !strcmp(endOfFile, "close") ) {
			endOfFile = strtok(fileBuffer, " ");
			if( !strcmp(endOfFile, fileName) )
				flag = 0;
		/* forward to others in chat */
		if( clntAddr1 != NULL ) {
			if( sendto(clntSock1, fileBuffer, sizeof(fileBuffer), 0, (struct sockaddr *)&clntAddr1, sizeof(clntAddr1) ) <0 )
			       DieWithError("Sendto() failed when forwarding to first client");	
		}
		if (clntAddr2 != NULL ) {
			if( sendto(clntSock2, fileBuffer, sizeof(fileBuffer), 0, (struct sockaddr *)&clntAddr2, sizeof(clntAddr2) ) <0 )
			       DieWithError("Sendto() failed when forwarding to first client");	
		}
	}
}
