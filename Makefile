nss:
	gcc -Wall -Wextra example.c -lnss3 -I /usr/include/nss -I /usr/include/nspr && ./a.out
clean:
	rm -rf *.o *.out
