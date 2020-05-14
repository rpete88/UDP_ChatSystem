/* DieWithError- Chat Program over UDP
 * author: Ryan Peterson
 *
 * Contains code to exit given a
 * 	specified error message.
 *
 */

#include <stdio.h> /* for perror() */
#include <stdlib.h> /* for exit() */

void DieWithError(char *errorMessage)
{
	perror(errorMessage);
	exit(1);
}

