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

bld r0,0
bld r31,7
bld r16,3

brbc 0,.+124
brbc 7,.-128
brbc 7,.+124

brbs 0,.+124
brbs 7,.-128
brbs 7,.+124
brbs 0,.-128

brcc .+124
brcc .-128

brcs .+124
brcs .-128

break

breq .+124
breq .+0

brge .-128

brhc .+4

brhs .+16

brid .+20

brie .+32

brlo .-64

brlt .+60

brmi .-64

brne .+64

brpl .+124

brsh .-124

brtc .+124

brts .-92

brvc .+64

brvs .+64

bset 0
bset 7
bset 3

bst r0, 0
bst r31, 0
bst r0, 7
bst r31, 7

call 0
call 0x1fffe

cbi 0,0
cbi 31,0
cbi 31,7

cbr r16, 0
cbr r16, 7
cbr r31, 0
cbr r31, 7

clc
clh
cli
cln

clr r0
clr r31

cls
clt
clv
clz

com r0
com r31

cp r0,r0
cp r31,r31
cp r0,r31

cpc r0,r0
cpc r31,r31
cpc r0,r31

cpi r16,0
cpi r16,255
cpi r31,255
cpi r31,0

cpse r0,r0
cpse r31,r31
cpse r0,r31

dec r0
dec r16

eor r0, r31
eor r0, r17
eor r29,r14
