#include <stdio.h>
#include <string.h>

/* pam関連*/
#define PAM_SM_AUTH
#define _PAM_EXTERN_FUNCTIONS

#include <security/pam_appl.h>
#include <security/pam_modules.h>

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <syslog.h>

#include "google_auth.h"

#define BUF_LEN 256

/* パスワード認証用メソッド */
PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  	int ret ;
  	const char *username ;
  	const char passwdmsg[32] = "Google password: " ;
  	const char loginmsg[32] = "Googel Auth OK! " ;
  	const char prompt1[32] = "Google accout: " ;
	char user[128];
  	char passwd1[128] ;
  	struct pam_conv *conv ;
  	struct pam_message msg[3] ;
  	struct pam_message *pmsg[3] ;
  	struct pam_response *resp ;
  	FILE *fp ;
	char err_message[BUF_LEN] ;

	openlog("PAM GoogleAuth",LOG_CONS | LOG_PID, LOG_USER);

	fp = fopen("/tmp/pam.log", "a") ;
  	if (fp == NULL){
		// この認証は無視する。
		syslog(LOG_ERR,"fopen error.");
		closelog();
		return PAM_SUCCESS ;
	}
	fputs("fopen succcess \n",fp);
 
 	/* ssh ユーザ名を取り出す */
  	ret = pam_get_user(pamh, &username, "Login: ") ;
  	if (ret != PAM_SUCCESS) {
		sprintf(err_message,"pam_get_user() function error: %d\n" ,ret) ;
		fputs(err_message,fp) ;
    	fclose(fp) ;
    	return PAM_SERVICE_ERR ;
  	}
   	if (strcmp(username, "nukanobu")==0) {
    	fclose(fp) ;
    	return PAM_SUCCESS ;
  	}
  	if (strcmp(username, "root")==0) {
		fputs("root no login.\n",fp);
    	fclose(fp) ;
    	return PAM_AUTH_ERR ;
  	}
  	fputs("ssh Username: ",fp);
  	fputs(username,fp);
  	fputs("\n",fp);
  
  	/* パスワード入力プロンプトを表示するメソッド(conv)を取り出す */
	ret = pam_get_item(pamh, PAM_CONV, (const void **)&conv) ;
  	if (ret != PAM_SUCCESS) {
    	fclose(fp) ;
    	return PAM_SERVICE_ERR ;
  	}

   	/* PAM_TEXT_INFOはメッセージを表示するだけ */
   	pmsg[0] = &msg[0] ;
  	msg[0].msg_style = PAM_PROMPT_ECHO_ON ;
  	msg[0].msg = prompt1 ;
  	pmsg[1] = &msg[1] ;
  	msg[1].msg_style = PAM_PROMPT_ECHO_OFF ;
  	msg[1].msg = passwdmsg ;
  
  	/* convメソッドを呼び出してパスワード入力を促す */
  	ret = conv->conv(2, (const struct pam_message **)pmsg,
                   &resp, conv->appdata_ptr) ;
  	if (ret != PAM_SUCCESS) {
    	fclose(fp) ;
    	return PAM_SERVICE_ERR ;
  	}
  	if (resp == NULL) {
    	fclose(fp) ;
    	return PAM_SERVICE_ERR ;
  	}

  	/* 入力したパスワードの数だけ値が戻る */
	// アカウント取得
	if (resp[0].resp != NULL) {
    	strcpy(user, resp[0].resp) ;
		syslog(LOG_ERR,"user=%s",user);
  	}else{
    	fputs("Wrong password: \n", fp);
    	fclose(fp);
    	return PAM_SERVICE_ERR ;
  	}
	// パスワード取得
	if (resp[1].resp != NULL){
		strcpy(passwd1,resp[1].resp);
		syslog(LOG_ERR,"passwd1=%s",passwd1);
	}else{
    	fputs("Wrong password: \n", fp);
    	fclose(fp);
    	return PAM_SERVICE_ERR ;
	} 
	// 認証処理
   	ret=google_auth(user,passwd1);
	syslog(LOG_ERR,"google_auth ret=%d", ret);
	if ( ret == G_A_AUTH_FAILED ){
		fprintf(fp,"google_auth return code=%d \n",ret);
		fclose(fp);
		return PAM_SERVICE_ERR ;
	}

  	/* PAM_TEXT_INFOだけならメッセージを表示するのみ */
	//認証OKメッセージ
	pmsg[0] = &msg[0] ;
  	msg[0].msg_style = PAM_TEXT_INFO ;
  	msg[0].msg = loginmsg ;
  	ret = conv->conv(1, (const struct pam_message **)pmsg,
                   &resp, conv->appdata_ptr) ;
  	if (ret != PAM_SUCCESS) {
    	fclose(fp) ;
    	return PAM_SERVICE_ERR ;
  	}

	fclose(fp) ;
  	return PAM_SUCCESS ;
}

/* パスワード設定用メソッド */
PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  return PAM_SUCCESS;
}

/* モジュール名を定義？ */
#ifdef PAM_STATIC

struct pam_module _pam_google_auth_modstruct = {
     "pam_google_auth",
     pam_sm_authenticate,
     pam_sm_setcred,
     NULL,
     NULL,
     NULL,
     NULL,
};

#endif
