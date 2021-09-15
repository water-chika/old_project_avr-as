avr-as: as.c
	gcc -g as.c -o avr-as

.PHONY: test

test: avr-as
	cd test; make

clean:
	rm --force avr-as
	cd test; make clean
