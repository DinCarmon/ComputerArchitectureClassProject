add $r2, $imm, $zero, 1       # $r2 = 0. $r2 shall hold the $required $remainder if we wish to increase the counter
add $r3, $imm, $zero, 128     # $r3 = 128. $r3 shall store the number of times we need to increase the counter
add $r4, $zero, $zero, 0       # $r4 = 0. $r4 shall store the number of times we increased the counter
a:
lw $r5, $zero, $zero, 0     # $r5 = mem[0]. $r5 holds the value in counter
sll $r6, $r5, $imm, 30      # shift left 30 bits. now only 2 last bits are present in $r6
sra $r6, $r6, $imm, 30      # $r6 = counter % 4. $r6 holds the $remainder
bne $imm, $r6, $r2, a       # if $remainder == $required $remainder increase value. We jump to instruction 3 because of delay slot
add $zero, $zero, $zero, 0       # no operation
add $r5, $r5, $imm, 1       # increase counter
add $r4, $r4, $imm, 1       # increase number of times we increased the counter
sw $r5, $zero, $zero, 0        # store new counter
bne $imm, $r3, $r4, a       # if we increased the counter 128 times
add $zero, $zero, $zero, 0       # no operation
halt $zero, $zero, $zero, 0      # halt program