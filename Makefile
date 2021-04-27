all:
	gcc -Wall -Wextra -I/usr/include/nspr -I/home/mpech/dist/private/nss/ -I/home/mpech/dist/public/nss main.c && ./a.out
clean:
	rm -rf *.o *.out
