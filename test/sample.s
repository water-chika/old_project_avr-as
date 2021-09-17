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


fmul r16,r16
fmul r23, r23
fmul r16, r23

fmuls r16, r16
fmuls r23,r23
fmuls r23, r16

fmulsu r16,r16
fmulsu r23,r23
fmulsu r16,r20

icall

ijmp

in r0, 0
in r0,63
in r31,63

inc r0
inc r31

jmp 0
jmp 0x1fffe

ld r0,x
ld r31,x+
ld r19,-x

ld r31,y
ld r19,y+
ld r9,-y
ldd r17,y+63
ldd r31, y+0


ld r31,z
ld r19,z+
ld r9,-z
ldd r17,z+63
ldd r31, z+0

ldi r16, 255
ldi r31, 0
ldi r31, 255

lds r0, 0
lds r31, 65535
lds r14, 33333

lpm
lpm r0,z
lpm r31,z+

lsr r1
lsr r30

mov r0,r7
mov r31,r0

movw r8,r0
movw r28, r2

mul r0,r7
mul r31,r21

muls r16,r19
muls r18,r22

mulsu r17,r17
mulsu r23,r21

neg r0
neg r31

nop

or r0,r31
or r31,r0

ori r16,1
ori r31,255

out 63, r0
out 0, r31
