# Compile flags for normal build
CC = gcc

ifndef PKGBUILD
RPATH = -Wl,--rpath,
endif

# Compile flags for debugging
# CFLAGS = -g -DDEBUG -Wall -D_REENTRANT $(LFSDEF) $(OSDEF)

# By default, Xymon uses a static library for common code.
# To save some diskspace and run-time memory, you can use a
# shared library by un-commenting this.
# XYMONLIBRARY=libxymon.so

# Mail program: This must support "CMD -s SUBJECT ADDRESS" to send out a mail with a subject
# Typically, this will be "mail" or "mailx"

all: libgoogleauth.so pam_google_auth.so
libgoogleauth.so: google_auth.c
	$(CC) -shared google_auth.c -o libgoogleauth.so -lssl -lcrypto -fPIC

pam_google_auth.so:	pam_google_auth.c
	$(CC) -fPIC -shared -o pam_google_auth.so pam_google_auth.c -lpam -lgoogleauth -L/root/work/pam_module

#pam_google_auth.so: pam_google_auth.c
#    $(CC)  pam_google_auth.so $(CFLAGS) 

clean:
	rm -f libgoogleauth.so pam_google_auth.so

#install:	
