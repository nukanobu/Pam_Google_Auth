//*****************//
//* google_auth.h *//
//*****************//
#ifdef GOOGLE_AUTH_H
#define GOOGLE_AUTH_H

int google_auth( *char[] user, *char[] passwd );


#endif


#define G_A_GETHOSTBYNAME_ERROR		1		/* gethostbyname() function error */
#define G_A_SOCKET_ERROR			2		/* socket() function error */
#define G_A_CONNECT_ERROR			3		/* connetct function error */
#define G_A_SSL_CTX_NEW_ERROR		4       /* SSL_CTX_new function error */
#define G_A_SSL_NEW_ERROR			5		/* SSL_new function error */
#define G_A_SSL_SET_FD_ERROR		6		/* SSL_set_fd function error */
#define G_A_SSL_CONNECT_ERROR		7 		/* SSL_connect function error */
#define G_A_SSL_WRITE_ERROR			8		/* SSL_write function error */
#define G_A_SSL_READ_ERROR			9		/* SSL_read function error */
#define G_A_SSL_SHUTDOWN_ERROR		10		/* SSL_shutdown function error */
#define G_A_AUTH_OK					200		/* HTTP status is 200 = Auth OK  */
#define G_A_AUTH_FAILED				403		/* HTTP status is 403 = Auth NG  */

