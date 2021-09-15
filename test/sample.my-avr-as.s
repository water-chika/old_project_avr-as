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

brbc 0,62
brbc 7,-64
brbc 7,62

brbs 0,+62
brbs 7,-64
brbs 7,+62
brbs 0,-64

brcc +62
brcc -64

brcs +62
brcs -64

break

breq +62
breq +0

brge -64

brhc +2

brhs +8

brid +10

brie +16

brlo -32

brlt +30

brmi -32

brne +32

brpl +62

brsh -62

brtc +62

brts -46

brvc +32

brvs +32

bset 0
bset 7
bset 3

bst r0, 0
bst r31, 0
bst r0, 7
bst r31, 7

call 0
call 0xffff

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

cpi r0,0
cpi r0,255
cpi r31,255
cpi r31,0

cpse r0,r0
cpse r31,r31
cpse r0,r31

dec r0
dec r16

des 0
des 0xf

eicall

eijmp

elpm r0, z
elpm r16, z+
elpm

eor r0, r31
eor r0, r17
eor r29,r14
