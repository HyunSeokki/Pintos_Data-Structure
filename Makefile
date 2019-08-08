testlib : main.o bitmap.o hash.o list.o
	gcc main.o bitmap.o hash.o list.o -o testlib
	rm *.o
main.o : main.c
	gcc -c main.c
bitmap.o : bitmap.h bitmap.c round.h limits.h stdio.c
	gcc -c bitmap.c
hash.o : hash.h hash.c
	gcc -c hash.c
list.o : list.h list.c
	gcc -c list.c
clean :
	rm testlib
