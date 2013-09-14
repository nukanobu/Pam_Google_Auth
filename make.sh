#!/bin/sh
#makefile じゃなくてごめんなさい。

ret=`rpm -qa | grep openssh`
if [ "${ret}" == "" ];then
	yum install openssh
fi
ret=`rpm -aq |grep openssl-devel`
if [ "${ret}" == "" ];then
    yum install openssl-devel
fi
ret=`rpm -qa |egrep ^gcc-[0-9]`
if [ "${ret}" == "" ];then
	yum install gcc
fi
ret=`rpm -qa |grep pam-devel`
if [ "${ret}" == "" ];then
	yum install pam-devel
fi
ret=`rpm -qa |grep libgcrypt`
if [ "${ret}" == "" ];then
	yum install libgcrypt
fi

#google_auth.c compile 
gcc -shared google_auth.c -o libgoogleauth.so -lssl -lcrypto -fPIC

cp -i libgoogleauth.so /lib64/

#pam_google_auth.c complie
gcc -fPIC -shared -o pam_google_auth.so pam_google_auth.c -lpam -lgoogleauth -L/root/work/pam_module

cp -i pam_google_auth.so /lib64/security/

echo ""
echo ""
echo "Change /etc/pam.d/sshd!"
echo "Sample. Add to a next line in sshd file."
echo ""
echo "auth       requisite    pam_google_auth.so"
echo ""
