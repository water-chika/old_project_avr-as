adc r0,r0
adc r31,r0
adc r0, r31
adc r31, r31

add r0, r0
add r31, r0
add r0, r31
add r31, r31

adiw r24, 1
adiw r26, 2
adiw r28, 31
adiw r30, 63

and r0, r0
and r31, r16

andi r16, 15
andi r31, 4

asr r0
asr r31

bclr 0
bclr 7
