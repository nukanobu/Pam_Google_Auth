# Compile flags for normal build
CC = gcc

ifndef PKGBUILD
RPATH = -Wl,--rpath,
endif

all: libgoogleauth.so pam_google_auth.so
libgoogleauth.so: google_auth.c
	$(CC) -shared google_auth.c -o libgoogleauth.so -lssl -lcrypto -fPIC

pam_google_auth.so:	pam_google_auth.c
	$(CC) -fPIC -shared -o pam_google_auth.so pam_google_auth.c -lpam -lgoogleauth -L/root/work/pam_module

clean:
	rm -f libgoogleauth.so pam_google_auth.so

install:
	cp libgoogleauth.so /lib64/
	cp pam_google_auth.so /lib64/security/

