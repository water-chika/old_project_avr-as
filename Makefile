avr-as: as.c
	gcc as.c -o avr-as

.PHONY: test

test: avr-as
	cd test; make
