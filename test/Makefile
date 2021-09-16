all: sample.bin sample.my-avr-as.bin
	avr-objdump --target=binary --architecture avr:5 --disassemble-all sample.bin > sample.bin.dump
	avr-objdump --target=binary --architecture avr:5 --disassemble-all sample.my-avr-as.bin > sample.my-avr-as.bin.dump
	if cmp sample.bin sample.my-avr-as.bin ; then \
		echo 'Test pass'; \
	else \
		diff sample.bin.dump sample.my-avr-as.bin.dump \
		echo 'Test fail'; \
	fi

sample.bin: sample.s
	avr-as -mmcu=atmega328 sample.s -o sample.elf.o
	avr-ld sample.elf.o -o sample.elf
	avr-objcopy --output-target binary sample.elf sample.bin

sample.my-avr-as.bin: sample.my-avr-as.s ../avr-as
	cat sample.my-avr-as.s | ../avr-as > sample.my-avr-as.bin

clean:
	rm --force sample.bin sample.my-avr-as.bin sample.elf sample.elf.o
