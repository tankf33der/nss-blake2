nss:
	gcc -Wall -Wextra main.c -lnss3 -I /usr/include/nss -I /usr/include/nspr && ./a.out
clean:
	rm -rf *.o *.out
