Lets test blake2 of NSS.

Have fun!

HOW TO REPEAT
=============
```
# setup environment
$ cd ~
$ hg clone https://hg.mozilla.org/projects/nss
$ cd nss
# install gyp and ninja
$ ./build.sh -v --target x64 --opt --system-sqlite --system-nspr --enable-libpkix --disable-tests
$ ls -l ~/dist
total 20
-rw-r--r-- 1 mpech mpech   88 apr 27 12:30 build_args
-rw-r--r-- 1 mpech mpech    8 apr 27 12:30 latest
drwxr-xr-x 3 mpech mpech 4096 apr 27 12:30 private
drwxr-xr-x 3 mpech mpech 4096 apr 27 12:30 public
drwxr-xr-x 4 mpech mpech 4096 apr 27 12:31 Release
$ find ~/dist -type f | xargs grep BLAKE2B_Hash
grep: /home/mpech/dist/Release/lib/libfreeblpriv3.so: binary file matches
/home/mpech/dist/private/nss/blapi.h:extern SECStatus BLAKE2B_Hash(unsigned char *dest, const char *src);
/home/mpech/dist/private/nss/blapi.h:extern SECStatus BLAKE2B_HashBuf(unsigned char *output,
$ 
```
