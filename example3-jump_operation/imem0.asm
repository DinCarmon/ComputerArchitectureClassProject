jal $imm, $zero, $zero, 3
add $r2, $r2, $imm, 19
add $r2, $r2, $imm, 22          # Should not occurr
halt $zero, $zero, $zero, 0