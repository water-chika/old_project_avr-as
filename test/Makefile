all: sample.bin sample.my-avr-as.bin
	if cmp --verbose --print-bytes sample.bin sample.my-avr-as.bin ; then \
		echo 'Test pass'; \
	else \
		echo 'Test fail'; \
	fi

sample.bin: sample.s
	avr-as -mmcu=avr5 sample.s -o sample.elf.o
	avr-ld sample.elf.o -o sample.elf
	avr-objcopy --output-target binary sample.elf sample.bin

sample.my-avr-as.bin: sample.my-avr-as.s ../avr-as
	cat sample.my-avr-as.s | ../avr-as > sample.my-avr-as.bin

clean:
	rm --force sample.bin sample.my-avr-as.bin sample.elf sample.elf.o
