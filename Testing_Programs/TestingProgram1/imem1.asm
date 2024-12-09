add r2, r1, r0, 1       # r2 = 0. r2 shall hold the required remainder if we wish to increase the counter
add r3, r1, r0, 128     # r3 = 128. r3 shall store the number of times we need to increase the counter
add r4, r0, r0, 0       # r4 = 0. r4 shall store the number of times we increased the counter
a: lw r5, r0, r0, 0     # r5 = mem[0]. r5 holds the value in counter
sll r6, r5, r1, 30      # shift left 30 bits. now only 2 last bits are present in r6
sra r6, r6, r1, 30      # r6 = counter % 4. r6 holds the remainder
bne r1, r6, r2, a       # if remainder == required remainder increase value. We jump to instruction 4 because of delay slot
add r0, r0, r0, 0       # no operation
add r5, r5, r1, 1       # increase counter
add r4, r4, r1, 1       # increase number of times we increased the counter
sw r5, r0, r0, 0        # store new counter
bne r1, r3, r4, a       # if we increased the counter 128 times
add r0, r0, r0, 0       # no operation
add r7, r0, r1, 1       # r7 = 1
sll r7, r7, r1, 8       # r7 = r7 * 2^8 = 2^8
lw r8, r7, r0, 0        # cause a conflict miss and write to main memory the value of counter.
halt r0, r0, r0, 0      # halt program