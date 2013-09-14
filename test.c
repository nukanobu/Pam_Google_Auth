//main.c
#include <stdio.h>
#include <string.h>
#include <resolv.h>
#include "google_auth.h"
int main(void){
	int ret;
	char username[25];
	char passwd[25];
	
	res_state resp = &_res;
	resp->retrans = 1;
	resp->retry = 1;
	printf("################\n");
	ret=google_auth("user@gmail.com","passwd");
	printf("################\n");
	printf("ret=%d \n",ret);
	return 0;

}

	
