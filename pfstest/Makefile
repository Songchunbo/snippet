
OBJS=pfs_server.o pfs_alloc.o
PROG=pfs_server pfs_alloc_test
#PROG=pfs_server

#LIBEASYDIR=/home/workspace/opensource/libeasy

#INCL=-I${LIBEASYDIR}/include  -I${LIBEASYDIR}/memory  -I${LIBEASYDIR}/util  -I${LIBEASYDIR}/thread  -I${LIBEASYDIR}/io  -I${LIBEASYDIR}/packet/http -I${LIBEASYDIR}/.
INCL=-I./include
LIBS=lib/libeasy.a -lpthread -lm

all: ${PROG}

pfs_server: ${OBJS}
	gcc -g ${OBJS} ${LIBS} -o $@

pfs_alloc_test: pfs_alloc.o pfs_alloc_test.o
	gcc -g $^ -o $@

pfs_server.o: pfs_server.c
	gcc -g ${INCL} -c $< -o $@

pfs_alloc.o: pfs_alloc.c
	gcc -g -c $< -o $@

pfs_alloc_test.o: pfs_alloc_test.c
	gcc -g -c $< -o $@
clean:
	rm -f *.o ${PROG}
