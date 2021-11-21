avr-as: as.o mnemonic.o
	gcc -g as.o mnemonic.o -o avr-as

as.o: as.c
	gcc -c as.c

mnemonic.o: mnemonic.c
	gcc -c mnemonic.c

.PHONY: test

test: avr-as
	cd test; make

clean:
	rm --force avr-as mnemonic.o as.o
	cd test; make clean
