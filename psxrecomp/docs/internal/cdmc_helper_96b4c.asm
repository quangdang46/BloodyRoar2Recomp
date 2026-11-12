0x80196b4c: addiu      $sp, $sp, -0x40
0x80196b50: sw         $s7, 0x34($sp)
0x80196b54: move       $s7, $a0
0x80196b58: sw         $s3, 0x24($sp)
0x80196b5c: move       $s3, $a1
0x80196b60: addiu      $a0, $zero, -1
0x80196b64: sw         $ra, 0x3c($sp)
0x80196b68: sw         $fp, 0x38($sp)
0x80196b6c: sw         $s6, 0x30($sp)
0x80196b70: sw         $s5, 0x2c($sp)
0x80196b74: sw         $s4, 0x28($sp)
0x80196b78: sw         $s2, 0x20($sp)
0x80196b7c: sw         $s1, 0x1c($sp)
0x80196b80: jal        0x8019a460
0x80196b84: sw         $s0, 0x18($sp)
0x80196b88: lui        $fp, 0x801c
0x80196b8c: addiu      $fp, $fp, -0x3794
0x80196b90: lui        $s5, 0x801c
0x80196b94: addiu      $s5, $s5, -0x3714
0x80196b98: lui        $s2, 0x801c
0x80196b9c: addiu      $s2, $s2, -0x34dc
0x80196ba0: addiu      $s6, $s2, 1
0x80196ba4: addiu      $s4, $s2, 2
0x80196ba8: addiu      $v0, $v0, 0x3c0
0x80196bac: lui        $at, 0x8020
0x80196bb0: sw         $v0, -0x3920($at)
0x80196bb4: lui        $v0, 0x8012
0x80196bb8: addiu      $v0, $v0, 0x5834
0x80196bbc: lui        $at, 0x8020
0x80196bc0: sw         $zero, -0x391c($at)
0x80196bc4: lui        $at, 0x8020
0x80196bc8: sw         $v0, -0x3918($at)
0x80196bcc: jal        0x8019a460
0x80196bd0: addiu      $a0, $zero, -1
0x80196bd4: lui        $v1, 0x8020
0x80196bd8: lw         $v1, -0x3920($v1)
0x80196bdc: nop        
0x80196be0: slt        $v1, $v1, $v0
0x80196be4: bnez       $v1, 0x80196c18
0x80196be8: nop        
0x80196bec: lui        $v0, 0x8020
0x80196bf0: lw         $v0, -0x391c($v0)
0x80196bf4: nop        
0x80196bf8: move       $v1, $v0
0x80196bfc: addiu      $v0, $v0, 1
0x80196c00: lui        $at, 0x8020
0x80196c04: sw         $v0, -0x391c($at)
0x80196c08: lui        $v0, 0x3c
0x80196c0c: slt        $v0, $v0, $v1
0x80196c10: beqz       $v0, 0x80196c80
0x80196c14: nop        
0x80196c18: lui        $a0, 0x8012
0x80196c1c: jal        0x80197ad0
0x80196c20: addiu      $a0, $a0, 0x57a4
0x80196c24: lbu        $a0, ($s2)
0x80196c28: lbu        $v0, 1($s2)
0x80196c2c: lui        $a1, 0x8020
0x80196c30: lw         $a1, -0x3918($a1)
0x80196c34: sll        $v0, $v0, 2
0x80196c38: addu       $v0, $v0, $s5
0x80196c3c: sll        $a0, $a0, 2
0x80196c40: lw         $v1, ($v0)
0x80196c44: lui        $v0, 0x801c
0x80196c48: lbu        $v0, -0x379b($v0)
0x80196c4c: addu       $a0, $a0, $s5
0x80196c50: sll        $v0, $v0, 2
0x80196c54: addu       $v0, $v0, $fp
0x80196c58: sw         $v1, 0x10($sp)
0x80196c5c: lw         $a2, ($v0)
0x80196c60: lw         $a3, ($a0)
0x80196c64: lui        $a0, 0x8012
0x80196c68: jal        0x8018e550
0x80196c6c: addiu      $a0, $a0, 0x57b4
0x80196c70: jal        0x801972a8
0x80196c74: nop        
0x80196c78: j          0x80196c84
0x80196c7c: addiu      $v0, $zero, -1
0x80196c80: move       $v0, $zero
0x80196c84: bnez       $v0, 0x80196de4
0x80196c88: addiu      $v0, $zero, -1
0x80196c8c: jal        0x8019a7d4
0x80196c90: nop        
0x80196c94: beqz       $v0, 0x80196d3c
0x80196c98: nop        
0x80196c9c: lui        $v0, 0x801c
0x80196ca0: lw         $v0, -0x34f4($v0)
0x80196ca4: nop        
0x80196ca8: lbu        $v0, ($v0)
0x80196cac: nop        
0x80196cb0: andi       $s1, $v0, 3
0x80196cb4: jal        0x80196370
0x80196cb8: nop        
0x80196cbc: move       $s0, $v0
0x80196cc0: beqz       $s0, 0x80196d2c
0x80196cc4: andi       $v0, $s0, 4
0x80196cc8: beqz       $v0, 0x80196cf8
0x80196ccc: andi       $v0, $s0, 2
0x80196cd0: lui        $v0, 0x801c
0x80196cd4: lw         $v0, -0x37b4($v0)
0x80196cd8: nop        
0x80196cdc: beqz       $v0, 0x80196cf4
0x80196ce0: nop        
0x80196ce4: lbu        $a0, ($s6)
0x80196ce8: lui        $a1, 0x8020
0x80196cec: jalr       $v0
0x80196cf0: addiu      $a1, $a1, -0x3930
0x80196cf4: andi       $v0, $s0, 2
0x80196cf8: beqz       $v0, 0x80196cb4
0x80196cfc: nop        
0x80196d00: lui        $v0, 0x801c
0x80196d04: lw         $v0, -0x37b8($v0)
0x80196d08: nop        
0x80196d0c: beqz       $v0, 0x80196cb4
0x80196d10: nop        
0x80196d14: lbu        $a0, ($s2)
0x80196d18: lui        $a1, 0x8020
0x80196d1c: jalr       $v0
0x80196d20: addiu      $a1, $a1, -0x3938
0x80196d24: j          0x80196cb4
0x80196d28: nop        
0x80196d2c: lui        $v0, 0x801c
0x80196d30: lw         $v0, -0x34f4($v0)
0x80196d34: nop        
0x80196d38: sb         $s1, ($v0)
0x80196d3c: lbu        $v0, ($s4)
0x80196d40: nop        
0x80196d44: andi       $a2, $v0, 0xff
0x80196d48: beqz       $a2, 0x80196d8c
0x80196d4c: nop        
0x80196d50: sb         $zero, 2($s2)
0x80196d54: lui        $a0, 0x8020
0x80196d58: addiu      $a0, $a0, -0x3928
0x80196d5c: beqz       $s3, 0x80196dd4
0x80196d60: move       $a1, $s3
0x80196d64: addiu      $v1, $zero, 7
0x80196d68: addiu      $a3, $zero, -1
0x80196d6c: lbu        $v0, ($a0)
0x80196d70: addiu      $a0, $a0, 1
0x80196d74: addiu      $v1, $v1, -1
0x80196d78: sb         $v0, ($a1)
0x80196d7c: bne        $v1, $a3, 0x80196d6c
0x80196d80: addiu      $a1, $a1, 1
0x80196d84: j          0x80196de4
0x80196d88: move       $v0, $a2
0x80196d8c: lbu        $v0, -1($s4)
0x80196d90: nop        
0x80196d94: andi       $a2, $v0, 0xff
0x80196d98: beqz       $a2, 0x80196ddc
0x80196d9c: nop        
0x80196da0: sb         $zero, 1($s2)
0x80196da4: move       $a1, $s3
0x80196da8: lui        $a0, 0x8020
0x80196dac: addiu      $a0, $a0, -0x3930
0x80196db0: beqz       $a1, 0x80196dd4
0x80196db4: addiu      $v1, $zero, 7
0x80196db8: addiu      $a3, $zero, -1
0x80196dbc: lbu        $v0, ($a0)
0x80196dc0: addiu      $a0, $a0, 1
0x80196dc4: addiu      $v1, $v1, -1
0x80196dc8: sb         $v0, ($a1)
0x80196dcc: bne        $v1, $a3, 0x80196dbc
0x80196dd0: addiu      $a1, $a1, 1
0x80196dd4: j          0x80196de4
0x80196dd8: move       $v0, $a2
0x80196ddc: beqz       $s7, 0x80196bcc
0x80196de0: move       $v0, $zero
0x80196de4: lw         $ra, 0x3c($sp)
0x80196de8: lw         $fp, 0x38($sp)
0x80196dec: lw         $s7, 0x34($sp)
0x80196df0: lw         $s6, 0x30($sp)
0x80196df4: lw         $s5, 0x2c($sp)
0x80196df8: lw         $s4, 0x28($sp)
0x80196dfc: lw         $s3, 0x24($sp)
0x80196e00: lw         $s2, 0x20($sp)
0x80196e04: lw         $s1, 0x1c($sp)
0x80196e08: lw         $s0, 0x18($sp)
0x80196e0c: jr         $ra