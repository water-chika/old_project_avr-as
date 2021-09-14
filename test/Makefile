all: sample.bin sample.my-avr-as.bin
	if cmp --verbose --print-bytes sample.bin sample.my-avr-as.bin ; then \
		echo 'Test pass'; \
	else \
		echo 'Test fail'; \
	fi

sample.bin: sample.s
	avr-as sample.s -o sample.elf
	avr-objcopy --output-target binary sample.elf sample.bin

sample.my-avr-as.bin: sample.s ../avr-as
	cat sample.s | ../avr-as > sample.my-avr-as.bin
