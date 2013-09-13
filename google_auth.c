
/**************************************
 * google auth library 
 * heder file is google_auth.h
 *************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <syslog.h>

#include "google_auth.h"

#define BUF_LEN 256

int google_auth(user, passwd)
     char *user;
     char *passwd;
{
	int ret;
	int retval=999;
	int s;
	struct hostent *servhost; 
	struct sockaddr_in server;
	struct servent *service; 
	FILE *fp;
    
	SSL *ssl;
	SSL_CTX *ctx;

	char request[BUF_LEN];

	//syslog open
	openlog("GoogleAuth",LOG_CONS | LOG_PID, LOG_USER);
	
	//  リクエスト先ドメイン
	char *host = "www.google.com";
	//  URL のパス
	char *path = "/accounts/ClientLogin";


	servhost = gethostbyname(host);
	if ( servhost == NULL ){
		syslog(LOG_ERR,"[%s] から IP アドレスへの変換に失敗しました。\n", host);
		closelog();
    	return G_A_GETHOSTBYNAME_ERROR;
  	}

	bzero((char *)&server, sizeof(server));
	server.sin_family = AF_INET;

	bcopy(servhost->h_addr, (char *)&server.sin_addr, servhost->h_length);

	/* ポート番号取得 */
	service = getservbyname("https", "tcp");
	if ( service != NULL ){
    	server.sin_port = service->s_port;
	} else {
    /* 取得できなかったら、ポート番号を 443 に決め打ち */
   		server.sin_port = htons(443);
	}

	s = socket(AF_INET, SOCK_STREAM, 0); 
	if ( s < 0 ){
		syslog(LOG_ERR,"ソケットの生成に失敗しました。\n");
		closelog();
		return G_A_SOCKET_ERROR;
  	}

	if ( connect(s, (struct sockaddr*) &server, sizeof(server)) == -1 ){
		syslog(LOG_ERR,"connect に失敗しました。\n");
		closelog();
		return G_A_CONNECT_ERROR;
	}

  	/* ここからが SSL */

  	SSL_load_error_strings();
  	SSL_library_init();
	//使用するプロトコル決定 SSLv2,SSLv3,TLSv1
  	ctx = SSL_CTX_new(SSLv23_client_method());
  	if ( ctx == NULL ){
    	ERR_print_errors_fp(fp);
		syslog(LOG_ERR,"%s",fp);
		return G_A_SSL_CTX_NEW_ERROR;
  	}
  	//SSL構造体生成
  	ssl = SSL_new(ctx);
  	if ( ssl == NULL ){
    	ERR_print_errors_fp(fp);
		syslog(LOG_ERR,"%s",fp);
    	return G_A_SSL_NEW_ERROR;
  	}
	//ソケットとSSLの結びつけ
  	ret = SSL_set_fd(ssl, s);
  	if ( ret == 0 ){
    	ERR_print_errors_fp(fp);
		syslog(LOG_ERR,"%s",fp);
    	return G_A_SSL_SET_FD_ERROR;
  	}

	/* PRNG 初期化 */
  	RAND_poll();
  	while ( RAND_status() == 0 ){
    	unsigned short rand_ret = rand() % 65536;
    	RAND_seed(&rand_ret, sizeof(rand_ret));
  	}

  	/* SSL で接続 */
  	ret = SSL_connect(ssl);
  	if ( ret != 1 ){
      	ERR_print_errors_fp(fp);
		syslog(LOG_ERR,"%s",fp);
      	return G_A_SSL_CONNECT_ERROR;
  	}

  	/* リクエスト送信 */
	char test[1024];
	strcpy(test,"ccountType=HOSTED_OR_GOOGLE&Email=");
	strcat(test,user);
	strcat(test,"&Passwd=");
	strcat(test,passwd);
	strcat(test,"&service=cl&source=nukanobu-pam-1.00");
   	sprintf(request, 
	  	"POST %s HTTP/1.0\r\n"
      	"Content-length: %d\r\n"
	  	"Content-type: application/x-www-form-urlencoded\r\n\r\n"
      	"%s\r\n",
          path,strlen(test),test);

  	ret = SSL_write(ssl, request, strlen(request));
  	if ( ret < 1 ){
    	ERR_print_errors_fp(fp);
		syslog(LOG_ERR,"%s",fp);
    	return G_A_SSL_WRITE_ERROR;
  	}

  	while (1){
    	char buf[BUF_LEN];
    	int read_size;
		char *str;
    	read_size = SSL_read(ssl, buf, sizeof(buf)-1);
    
    	if ( read_size > 0 ){
      		buf[read_size] = '\0';
			str=strstr(buf,"200 OK");
			if(str!=NULL){
				retval=G_A_AUTH_OK;
				syslog(LOG_INFO,"str=%s",str);
			}else{
				//10番目の文字を抽出しhttpステータスをretvalに設定
				str=strstr(buf,"HTTP/");
				if(str!=NULL){
					char tmp[4];
					memset(tmp,0x00,sizeof(tmp));
					memcpy(tmp,&str[9],3);
					retval=atoi(tmp);
				}
			}

    	} else if ( read_size == 0 ){
      		/* FIN 受信 */
      		break;
    	} else {
      		ERR_print_errors_fp(fp);
			syslog(LOG_ERR,"%s",fp);
      		return G_A_SSL_READ_ERROR;
    	}
  	}

  	ret = SSL_shutdown(ssl); 
  	if ( ret > 1 || ret < 0 ){
    	ERR_print_errors_fp(fp);
    	syslog(LOG_ERR,"%s",fp);
    	return G_A_SSL_SHUTDOWN_ERROR;
  	}
  	close(s);

  	SSL_free(ssl); 
  	SSL_CTX_free(ctx);
  	ERR_free_strings();
 
  	closelog();
	return retval;
}
