all:	statically-linked

statically-linked:	main.o	msg_pdu.a
	gcc -o statically-linked main.o -L. msg_pdu.a
	rm -f	*.o	#if you dont want to remove the Object files then Comment this line with #

main.o:	main.c
	gcc -std=c11 -c main.c

msg_pdu.o:	msg_pdu.c
	gcc -std=c11 -c msg_pdu.c

msg_pdu.a:	msg_pdu.o
	ar rcs msg_pdu.a msg_pdu.o

#clean:
#	rm -f statically-linked *.o

