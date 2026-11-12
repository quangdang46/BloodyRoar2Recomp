0x80199440: addiu      $sp, $sp, -0x40
0x80199444: lui        $v0, 0x8020
0x80199448: lw         $v0, -0x3964($v0)
0x8019944c: addiu      $a0, $zero, 1
0x80199450: beq        $v0, $a0, 0x80199d4c
0x80199454: sw         $ra, 0x38($sp)
0x80199458: lui        $v0, 0x8020
0x8019945c: lw         $v0, -0x3978($v0)
0x80199460: nop        
0x80199464: beqz       $v0, 0x801994c4
0x80199468: nop        
0x8019946c: lui        $v0, 0x801c
0x80199470: lw         $v0, -0x33a0($v0)
0x80199474: nop        
0x80199478: lw         $v0, ($v0)
0x8019947c: lui        $v1, 0x100
0x80199480: and        $v0, $v0, $v1
0x80199484: beqz       $v0, 0x801994c4
0x80199488: nop        
0x8019948c: lui        $v0, 0x8020
0x80199490: lw         $v0, -0x3950($v0)
0x80199494: lui        $at, 0x8020
0x80199498: beqz       $v0, 0x801994b8
0x8019949c: sw         $a0, -0x3974($at)
0x801994a0: lui        $v0, 0x8020
0x801994a4: lw         $v0, -0x3960($v0)
0x801994a8: nop        
0x801994ac: addiu      $v0, $v0, 1
0x801994b0: lui        $at, 0x8020
0x801994b4: sw         $v0, -0x3960($at)
0x801994b8: lui        $at, 0x801c
0x801994bc: j          0x80199d4c
0x801994c0: sw         $a0, -0x3378($at)
0x801994c4: jal        0x80195d44
0x801994c8: addiu      $a1, $sp, 0x30
0x801994cc: addiu      $v1, $zero, 5
0x801994d0: beq        $v0, $v1, 0x80199d4c
0x801994d4: nop        
0x801994d8: lbu        $v0, 0x30($sp)
0x801994dc: lbu        $v1, 0x31($sp)
0x801994e0: sh         $v0, 0x22($sp)
0x801994e4: sh         $v1, 0x24($sp)
0x801994e8: lhu        $v0, 0x22($sp)
0x801994ec: nop        
0x801994f0: andi       $v0, $v0, 4
0x801994f4: beqz       $v0, 0x80199508
0x801994f8: addiu      $v0, $zero, 3
0x801994fc: lui        $at, 0x801c
0x80199500: j          0x80199d4c
0x80199504: sw         $v0, -0x3378($at)
0x80199508: lui        $v0, 0x8020
0x8019950c: lw         $v0, -0x395c($v0)
0x80199510: lui        $v1, 0x8020
0x80199514: lw         $v1, -0x3940($v1)
0x80199518: sll        $v0, $v0, 5
0x8019951c: addu       $v1, $v1, $v0
0x80199520: lui        $at, 0x8020
0x80199524: sw         $v1, -0x14e8($at)
0x80199528: lhu        $v0, ($v1)
0x8019952c: nop        
0x80199530: beqz       $v0, 0x80199574
0x80199534: nop        
0x80199538: lui        $v0, 0x8020
0x8019953c: lw         $v0, -0x3950($v0)
0x80199540: nop        
0x80199544: beqz       $v0, 0x80199568
0x80199548: addiu      $v0, $zero, 4
0x8019954c: lui        $v0, 0x8020
0x80199550: lw         $v0, -0x3960($v0)
0x80199554: nop        
0x80199558: addiu      $v0, $v0, 1
0x8019955c: lui        $at, 0x8020
0x80199560: sw         $v0, -0x3960($at)
0x80199564: addiu      $v0, $zero, 4
0x80199568: lui        $at, 0x801c
0x8019956c: j          0x80199d4c
0x80199570: sw         $v0, -0x3378($at)
0x80199574: lui        $v0, 0x801c
0x80199578: lw         $v0, -0x33c0($v0)
0x8019957c: nop        
0x80199580: sb         $zero, ($v0)
0x80199584: lui        $v0, 0x801c
0x80199588: lw         $v0, -0x33b4($v0)
0x8019958c: nop        
0x80199590: sb         $zero, ($v0)
0x80199594: lui        $v0, 0x801c
0x80199598: lw         $v0, -0x33c0($v0)
0x8019959c: lui        $a0, 2
0x801995a0: sb         $zero, ($v0)
0x801995a4: lui        $v1, 0x801c
0x801995a8: lw         $v1, -0x33b4($v1)
0x801995ac: addiu      $v0, $zero, 0x80
0x801995b0: sb         $v0, ($v1)
0x801995b4: lui        $v0, 0x801c
0x801995b8: lw         $v0, -0x33b0($v0)
0x801995bc: ori        $a0, $a0, 0x943
0x801995c0: sw         $a0, ($v0)
0x801995c4: lui        $v1, 0x801c
0x801995c8: lw         $v1, -0x33ac($v1)
0x801995cc: addiu      $v0, $zero, 0x1323
0x801995d0: sw         $v0, ($v1)
0x801995d4: lui        $v0, 0x8020
0x801995d8: lw         $v0, -0x3908($v0)
0x801995dc: nop        
0x801995e0: bnez       $v0, 0x80199634
0x801995e4: move       $a0, $zero
0x801995e8: addiu      $a1, $sp, 0x28
0x801995ec: lui        $v0, 0x801c
0x801995f0: lw         $v0, -0x33b8($v0)
0x801995f4: addu       $v1, $a1, $a0
0x801995f8: lbu        $v0, ($v0)
0x801995fc: addiu      $a0, $a0, 1
0x80199600: sb         $v0, ($v1)
0x80199604: sltiu      $v0, $a0, 4
0x80199608: bnez       $v0, 0x801995ec
0x8019960c: nop        
0x80199610: move       $a0, $zero
0x80199614: lui        $v1, 0x801c
0x80199618: lw         $v1, -0x33b8($v1)
0x8019961c: nop        
0x80199620: lbu        $v0, ($v1)
0x80199624: addiu      $a0, $a0, 1
0x80199628: sltiu      $v0, $a0, 8
0x8019962c: bnez       $v0, 0x80199620
0x80199630: nop        
0x80199634: lui        $v0, 0x8020
0x80199638: lw         $v0, -0x3950($v0)
0x8019963c: nop        
0x80199640: beqz       $v0, 0x80199674
0x80199644: lui        $t0, 0x1100
0x80199648: addiu      $a2, $zero, 8
0x8019964c: move       $a3, $zero
0x80199650: lui        $a1, 0x8020
0x80199654: lw         $a1, -0x3960($a1)
0x80199658: lui        $a0, 0x8020
0x8019965c: lw         $a0, -0x14e8($a0)
0x80199660: sll        $a1, $a1, 0xb
0x80199664: jal        0x80199d5c
0x80199668: addu       $a1, $v0, $a1
0x8019966c: j          0x80199698
0x80199670: nop        
0x80199674: addiu      $a0, $zero, 3
0x80199678: move       $a2, $zero
0x8019967c: lui        $a1, 0x8020
0x80199680: lw         $a1, -0x14e8($a1)
0x80199684: addiu      $a3, $zero, 8
0x80199688: sw         $t0, 0x10($sp)
0x8019968c: sw         $zero, 0x14($sp)
0x80199690: jal        0x80199d88
0x80199694: sw         $zero, 0x18($sp)
0x80199698: lui        $a0, 0x801c
0x8019969c: lw         $a0, -0x3390($a0)
0x801996a0: nop        
0x801996a4: lw         $v0, ($a0)
0x801996a8: lui        $v1, 0x100
0x801996ac: and        $v0, $v0, $v1
0x801996b0: beqz       $v0, 0x801996d0
0x801996b4: move       $v1, $a0
0x801996b8: lui        $a0, 0x100
0x801996bc: lw         $v0, ($v1)
0x801996c0: nop        
0x801996c4: and        $v0, $v0, $a0
0x801996c8: bnez       $v0, 0x801996bc
0x801996cc: nop        
0x801996d0: lui        $a0, 2
0x801996d4: ori        $a0, $a0, 0x843
0x801996d8: lui        $v0, 0x8020
0x801996dc: lw         $v0, -0x14e8($v0)
0x801996e0: lui        $v1, 0x801c
0x801996e4: lw         $v1, -0x33b0($v1)
0x801996e8: lwl        $a1, 0x2b($sp)
0x801996ec: lwr        $a1, 0x28($sp)
0x801996f0: nop        
0x801996f4: swl        $a1, 0x1f($v0)
0x801996f8: swr        $a1, 0x1c($v0)
0x801996fc: sw         $a0, ($v1)
0x80199700: lui        $v1, 0x801c
0x80199704: lw         $v1, -0x33ac($v1)
0x80199708: addiu      $v0, $zero, 0x1325
0x8019970c: sw         $v0, ($v1)
0x80199710: lui        $v1, 0x8020
0x80199714: lw         $v1, -0x3948($v1)
0x80199718: addiu      $v0, $zero, 1
0x8019971c: bne        $v1, $v0, 0x80199790
0x80199720: nop        
0x80199724: lui        $a0, 0x8020
0x80199728: lw         $a0, -0x396c($a0)
0x8019972c: nop        
0x80199730: beqz       $a0, 0x80199790
0x80199734: nop        
0x80199738: lui        $v1, 0x8020
0x8019973c: lw         $v1, -0x14e8($v1)
0x80199740: nop        
0x80199744: lhu        $v0, 8($v1)
0x80199748: nop        
0x8019974c: beq        $a0, $v0, 0x80199788
0x80199750: nop        
0x80199754: sh         $zero, ($v1)
0x80199758: lui        $v0, 0x8020
0x8019975c: lw         $v0, -0x3950($v0)
0x80199760: nop        
0x80199764: beqz       $v0, 0x80199d4c
0x80199768: nop        
0x8019976c: lui        $v0, 0x8020
0x80199770: lw         $v0, -0x3960($v0)
0x80199774: nop        
0x80199778: addiu      $v0, $v0, 1
0x8019977c: lui        $at, 0x8020
0x80199780: j          0x80199d4c
0x80199784: sw         $v0, -0x3960($at)
0x80199788: lui        $at, 0x8020
0x8019978c: sw         $zero, -0x3948($at)
0x80199790: lui        $a0, 0x8020
0x80199794: lw         $a0, -0x14e8($a0)
0x80199798: nop        
0x8019979c: lhu        $v1, ($a0)
0x801997a0: addiu      $v0, $zero, 0x160
0x801997a4: bne        $v1, $v0, 0x801997c8
0x801997a8: nop        
0x801997ac: lhu        $v0, 2($a0)
0x801997b0: lui        $v1, 0x8020
0x801997b4: lw         $v1, -0x3968($v1)
0x801997b8: srl        $v0, $v0, 0xa
0x801997bc: andi       $v0, $v0, 0x1f
0x801997c0: beq        $v0, $v1, 0x80199808
0x801997c4: nop        
0x801997c8: lui        $v0, 0x8020
0x801997cc: lw         $v0, -0x3950($v0)
0x801997d0: nop        
0x801997d4: beqz       $v0, 0x801997e8
0x801997d8: nop        
0x801997dc: lui        $at, 0x8020
0x801997e0: j          0x801997ec
0x801997e4: sw         $zero, -0x3960($at)
0x801997e8: lhu        $v0, ($a0)
0x801997ec: lui        $v1, 0x8020
0x801997f0: lw         $v1, -0x14e8($v1)
0x801997f4: addiu      $v0, $zero, 5
0x801997f8: lui        $at, 0x801c
0x801997fc: sw         $v0, -0x3378($at)
0x80199800: j          0x80199d4c
0x80199804: sh         $zero, ($v1)
0x80199808: lui        $v1, 0x8020
0x8019980c: lh         $v1, -0x397c($v1)
0x80199810: lhu        $v0, 4($a0)
0x80199814: nop        
0x80199818: bne        $v1, $v0, 0x80199844
0x8019981c: nop        
0x80199820: lui        $v1, 0x8020
0x80199824: lw         $v1, -0x3980($v1)
0x80199828: nop        
0x8019982c: beqz       $v1, 0x801998c4
0x80199830: nop        
0x80199834: lhu        $v0, 8($a0)
0x80199838: nop        
0x8019983c: beq        $v1, $v0, 0x801998c4
0x80199840: nop        
0x80199844: lui        $a0, 0x8020
0x80199848: lw         $a0, -0x3958($a0)
0x8019984c: lui        $a1, 0x8020
0x80199850: lw         $a1, -0x395c($a1)
0x80199854: lui        $at, 0x8020
0x80199858: sw         $zero, -0x3980($at)
0x8019985c: lui        $at, 0x8020
0x80199860: sh         $zero, -0x397c($at)
0x80199864: jal        0x80199320
0x80199868: subu       $a1, $a1, $a0
0x8019986c: lui        $v0, 0x8020
0x80199870: lw         $v0, -0x3958($v0)
0x80199874: lui        $v1, 0x8020
0x80199878: lw         $v1, -0x14e8($v1)
0x8019987c: lui        $at, 0x8020
0x80199880: sw         $v0, -0x395c($at)
0x80199884: sh         $zero, ($v1)
0x80199888: lui        $v0, 0x8020
0x8019988c: lw         $v0, -0x3950($v0)
0x80199890: nop        
0x80199894: beqz       $v0, 0x801998b8
0x80199898: addiu      $v0, $zero, 6
0x8019989c: lui        $v0, 0x8020
0x801998a0: lw         $v0, -0x3960($v0)
0x801998a4: nop        
0x801998a8: addiu      $v0, $v0, 1
0x801998ac: lui        $at, 0x8020
0x801998b0: sw         $v0, -0x3960($at)
0x801998b4: addiu      $v0, $zero, 6
0x801998b8: lui        $at, 0x801c
0x801998bc: j          0x80199d4c
0x801998c0: sw         $v0, -0x3378($at)
0x801998c4: lui        $v1, 0x8020
0x801998c8: lw         $v1, -0x14e8($v1)
0x801998cc: nop        
0x801998d0: lhu        $v0, 4($v1)
0x801998d4: nop        
0x801998d8: bnez       $v0, 0x80199b10
0x801998dc: addiu      $v0, $zero, 0xa
0x801998e0: lhu        $v0, 8($v1)
0x801998e4: lui        $v1, 0x8020
0x801998e8: lw         $v1, -0x394c($v1)
0x801998ec: lui        $at, 0x8020
0x801998f0: sh         $zero, -0x397c($at)
0x801998f4: andi       $v0, $v0, 0xffff
0x801998f8: lui        $at, 0x8020
0x801998fc: sw         $v0, -0x3980($at)
0x80199900: beqz       $v1, 0x801999b0
0x80199904: sltu       $v0, $v0, $v1
0x80199908: bnez       $v0, 0x801999b0
0x8019990c: nop        
0x80199910: lui        $a0, 0x8020
0x80199914: lw         $a0, -0x3958($a0)
0x80199918: lui        $a1, 0x8020
0x8019991c: lw         $a1, -0x395c($a1)
0x80199920: lui        $at, 0x8020
0x80199924: sw         $zero, -0x3980($at)
0x80199928: lui        $at, 0x8020
0x8019992c: sh         $zero, -0x397c($at)
0x80199930: jal        0x80199320
0x80199934: subu       $a1, $a1, $a0
0x80199938: lui        $v0, 0x8020
0x8019993c: lw         $v0, -0x3958($v0)
0x80199940: lui        $v1, 0x8020
0x80199944: lw         $v1, -0x14e8($v1)
0x80199948: lui        $at, 0x8020
0x8019994c: sw         $v0, -0x395c($at)
0x80199950: sh         $zero, ($v1)
0x80199954: lui        $v1, 0x8020
0x80199958: lw         $v1, -0x14ec($v1)
0x8019995c: addiu      $v0, $zero, 1
0x80199960: lui        $at, 0x8020
0x80199964: beqz       $v1, 0x80199974
0x80199968: sw         $v0, -0x3948($at)
0x8019996c: jalr       $v1
0x80199970: nop        
0x80199974: lui        $v0, 0x8020
0x80199978: lw         $v0, -0x3950($v0)
0x8019997c: nop        
0x80199980: beqz       $v0, 0x801999a4
0x80199984: addiu      $v0, $zero, 7
0x80199988: lui        $v0, 0x8020
0x8019998c: lw         $v0, -0x3960($v0)
0x80199990: nop        
0x80199994: addiu      $v0, $v0, 1
0x80199998: lui        $at, 0x8020
0x8019999c: sw         $v0, -0x3960($at)
0x801999a0: addiu      $v0, $zero, 7
0x801999a4: lui        $at, 0x801c
0x801999a8: j          0x80199d4c
0x801999ac: sw         $v0, -0x3378($at)
0x801999b0: lui        $v0, 0x8020
0x801999b4: lw         $v0, -0x393c($v0)
0x801999b8: lui        $v1, 0x8020
0x801999bc: lw         $v1, -0x395c($v1)
0x801999c0: lui        $a0, 0x8020
0x801999c4: lw         $a0, -0x14e8($a0)
0x801999c8: subu       $v0, $v0, $v1
0x801999cc: lhu        $v1, 6($a0)
0x801999d0: addiu      $v0, $v0, -1
0x801999d4: sltu       $v0, $v0, $v1
0x801999d8: beqz       $v0, 0x80199afc
0x801999dc: nop        
0x801999e0: lui        $v0, 0x8020
0x801999e4: lw         $v0, -0x394c($v0)
0x801999e8: nop        
0x801999ec: bnez       $v0, 0x80199a54
0x801999f0: addiu      $v0, $zero, 1
0x801999f4: sh         $v0, ($a0)
0x801999f8: lui        $v1, 0x8020
0x801999fc: lw         $v1, -0x14ec($v1)
0x80199a00: addiu      $v0, $zero, 1
0x80199a04: lui        $at, 0x8020
0x80199a08: beqz       $v1, 0x80199a18
0x80199a0c: sw         $v0, -0x3948($at)
0x80199a10: jalr       $v1
0x80199a14: nop        
0x80199a18: lui        $v0, 0x8020
0x80199a1c: lw         $v0, -0x3950($v0)
0x80199a20: nop        
0x80199a24: beqz       $v0, 0x80199a48
0x80199a28: addiu      $v0, $zero, 8
0x80199a2c: lui        $v0, 0x8020
0x80199a30: lw         $v0, -0x3960($v0)
0x80199a34: nop        
0x80199a38: addiu      $v0, $v0, 1
0x80199a3c: lui        $at, 0x8020
0x80199a40: sw         $v0, -0x3960($at)
0x80199a44: addiu      $v0, $zero, 8
0x80199a48: lui        $at, 0x801c
0x80199a4c: j          0x80199d4c
0x80199a50: sw         $v0, -0x3378($at)
0x80199a54: lui        $v0, 0x8020
0x80199a58: lw         $v0, -0x3940($v0)
0x80199a5c: nop        
0x80199a60: lh         $v0, ($v0)
0x80199a64: nop        
0x80199a68: beqz       $v0, 0x80199ab0
0x80199a6c: addiu      $v0, $zero, 1
0x80199a70: sh         $zero, ($a0)
0x80199a74: lui        $v0, 0x8020
0x80199a78: lw         $v0, -0x3950($v0)
0x80199a7c: nop        
0x80199a80: beqz       $v0, 0x80199aa4
0x80199a84: addiu      $v0, $zero, 9
0x80199a88: lui        $v0, 0x8020
0x80199a8c: lw         $v0, -0x3960($v0)
0x80199a90: nop        
0x80199a94: addiu      $v0, $v0, 1
0x80199a98: lui        $at, 0x8020
0x80199a9c: sw         $v0, -0x3960($at)
0x80199aa0: addiu      $v0, $zero, 9
0x80199aa4: lui        $at, 0x801c
0x80199aa8: j          0x80199d4c
0x80199aac: sw         $v0, -0x3378($at)
0x80199ab0: sh         $v0, ($a0)
0x80199ab4: lui        $a1, 0x8020
0x80199ab8: lw         $a1, -0x3940($a1)
0x80199abc: lui        $v1, 0x8020
0x80199ac0: lw         $v1, -0x14e8($v1)
0x80199ac4: move       $a0, $zero
0x80199ac8: lui        $at, 0x8020
0x80199acc: sw         $zero, -0x395c($at)
0x80199ad0: lw         $v0, ($v1)
0x80199ad4: addiu      $v1, $v1, 4
0x80199ad8: addiu      $a0, $a0, 1
0x80199adc: sw         $v0, ($a1)
0x80199ae0: sltiu      $v0, $a0, 8
0x80199ae4: bnez       $v0, 0x80199ad0
0x80199ae8: addiu      $a1, $a1, 4
0x80199aec: lui        $v0, 0x8020
0x80199af0: lw         $v0, -0x3940($v0)
0x80199af4: lui        $at, 0x8020
0x80199af8: sw         $v0, -0x14e8($at)
0x80199afc: lui        $v0, 0x8020
0x80199b00: lw         $v0, -0x395c($v0)
0x80199b04: lui        $at, 0x8020
0x80199b08: sw         $v0, -0x3958($at)
0x80199b0c: addiu      $v0, $zero, 0xa
0x80199b10: lui        $at, 0x801c
0x80199b14: sw         $v0, -0x3378($at)
0x80199b18: lui        $v0, 0x8020
0x80199b1c: lhu        $v0, -0x397c($v0)
0x80199b20: lui        $a0, 0x8020
0x80199b24: lw         $a0, -0x393c($a0)
0x80199b28: lui        $v1, 0x8020
0x80199b2c: lw         $v1, -0x3940($v1)
0x80199b30: lui        $a1, 0x8020
0x80199b34: lw         $a1, -0x395c($a1)
0x80199b38: addiu      $v0, $v0, 1
0x80199b3c: sll        $a0, $a0, 5
0x80199b40: addu       $v1, $v1, $a0
0x80199b44: lui        $at, 0x8020
0x80199b48: sh         $v0, -0x397c($at)
0x80199b4c: sll        $v0, $a1, 6
0x80199b50: subu       $v0, $v0, $a1
0x80199b54: sll        $v0, $v0, 5
0x80199b58: lui        $a0, 0x8020
0x80199b5c: lw         $a0, -0x3978($a0)
0x80199b60: addu       $v1, $v1, $v0
0x80199b64: lui        $at, 0x8020
0x80199b68: sw         $v1, -0x3944($at)
0x80199b6c: beqz       $a0, 0x80199b9c
0x80199b70: lui        $t0, 0x1100
0x80199b74: lui        $v1, 2
0x80199b78: lui        $v0, 0x801c
0x80199b7c: lw         $v0, -0x33b0($v0)
0x80199b80: ori        $v1, $v1, 0x943
0x80199b84: sw         $v1, ($v0)
0x80199b88: lui        $v1, 0x801c
0x80199b8c: lw         $v1, -0x33ac($v1)
0x80199b90: addiu      $v0, $zero, 0x1323
0x80199b94: j          0x80199bb8
0x80199b98: sw         $v0, ($v1)
0x80199b9c: lui        $v1, 0x2102
0x80199ba0: ori        $v1, $v1, 0x843
0x80199ba4: lui        $t0, 0x1140
0x80199ba8: lui        $v0, 0x801c
0x80199bac: lw         $v0, -0x33b0($v0)
0x80199bb0: ori        $t0, $t0, 0x100
0x80199bb4: sw         $v1, ($v0)
0x80199bb8: lui        $v0, 0x8020
0x80199bbc: lw         $v0, -0x14e8($v0)
0x80199bc0: nop        
0x80199bc4: lhu        $v1, 6($v0)
0x80199bc8: lhu        $v0, 4($v0)
0x80199bcc: addiu      $v1, $v1, -1
0x80199bd0: bne        $v1, $v0, 0x80199c78
0x80199bd4: addiu      $v1, $zero, 1
0x80199bd8: lui        $v0, 0x8020
0x80199bdc: lw         $v0, -0x3950($v0)
0x80199be0: lui        $at, 0x8020
0x80199be4: sw         $v1, -0x3964($at)
0x80199be8: beqz       $v0, 0x80199c30
0x80199bec: addiu      $a2, $zero, 0x1f8
0x80199bf0: addiu      $a3, $zero, 1
0x80199bf4: lui        $a1, 0x8020
0x80199bf8: lw         $a1, -0x3960($a1)
0x80199bfc: lui        $a0, 0x8020
0x80199c00: lw         $a0, -0x3944($a0)
0x80199c04: sll        $a1, $a1, 0xb
0x80199c08: addu       $a1, $v0, $a1
0x80199c0c: jal        0x80199d5c
0x80199c10: addiu      $a1, $a1, 0x20
0x80199c14: lui        $v0, 0x8020
0x80199c18: lw         $v0, -0x3960($v0)
0x80199c1c: nop        
0x80199c20: addiu      $v0, $v0, 1
0x80199c24: lui        $at, 0x8020
0x80199c28: j          0x80199c54
0x80199c2c: sw         $v0, -0x3960($at)
0x80199c30: addiu      $a0, $zero, 3
0x80199c34: move       $a2, $zero
0x80199c38: lui        $a1, 0x8020
0x80199c3c: lw         $a1, -0x3944($a1)
0x80199c40: addiu      $a3, $zero, 0x1f8
0x80199c44: sw         $t0, 0x10($sp)
0x80199c48: sw         $v1, 0x14($sp)
0x80199c4c: jal        0x80199d88
0x80199c50: sw         $zero, 0x18($sp)
0x80199c54: lui        $v0, 0x8020
0x80199c58: lw         $v0, -0x3970($v0)
0x80199c5c: lui        $at, 0x8020
0x80199c60: sh         $zero, -0x397c($at)
0x80199c64: lui        $at, 0x8020
0x80199c68: sw         $zero, -0x3980($at)
0x80199c6c: lui        $at, 0x8020
0x80199c70: j          0x80199cf0
0x80199c74: sw         $v0, -0x3968($at)
0x80199c78: lui        $v0, 0x8020
0x80199c7c: lw         $v0, -0x3950($v0)
0x80199c80: nop        
0x80199c84: beqz       $v0, 0x80199ccc
0x80199c88: addiu      $a2, $zero, 0x1f8
0x80199c8c: move       $a3, $zero
0x80199c90: lui        $a1, 0x8020
0x80199c94: lw         $a1, -0x3960($a1)
0x80199c98: lui        $a0, 0x8020
0x80199c9c: lw         $a0, -0x3944($a0)
0x80199ca0: sll        $a1, $a1, 0xb
0x80199ca4: addu       $a1, $v0, $a1
0x80199ca8: jal        0x80199d5c
0x80199cac: addiu      $a1, $a1, 0x20
0x80199cb0: lui        $v0, 0x8020
0x80199cb4: lw         $v0, -0x3960($v0)
0x80199cb8: nop        
0x80199cbc: addiu      $v0, $v0, 1
0x80199cc0: lui        $at, 0x8020
0x80199cc4: j          0x80199cf0
0x80199cc8: sw         $v0, -0x3960($at)
0x80199ccc: addiu      $a0, $zero, 3
0x80199cd0: move       $a2, $zero
0x80199cd4: lui        $a1, 0x8020
0x80199cd8: lw         $a1, -0x3944($a1)
0x80199cdc: addiu      $a3, $zero, 0x1f8
0x80199ce0: sw         $t0, 0x10($sp)
0x80199ce4: sw         $zero, 0x14($sp)
0x80199ce8: jal        0x80199d88
0x80199cec: sw         $zero, 0x18($sp)
0x80199cf0: lui        $v1, 0x801c
0x80199cf4: lw         $v1, -0x33ac($v1)
0x80199cf8: addiu      $v0, $zero, 0x1325
0x80199cfc: sw         $v0, ($v1)
0x80199d00: lui        $v1, 0x8020
0x80199d04: lw         $v1, -0x14e8($v1)
0x80199d08: addiu      $v0, $zero, 3
0x80199d0c: sh         $v0, ($v1)
0x80199d10: lui        $v0, 0x8020
0x80199d14: lw         $v0, -0x395c($v0)
0x80199d18: lui        $v1, 0x8020
0x80199d1c: lw         $v1, -0x3950($v1)
0x80199d20: addiu      $v0, $v0, 1
0x80199d24: lui        $at, 0x8020
0x80199d28: beqz       $v1, 0x80199d4c
0x80199d2c: sw         $v0, -0x395c($at)
0x80199d30: lui        $v0, 0x8020
0x80199d34: lw         $v0, -0x3964($v0)
0x80199d38: nop        
0x80199d3c: beqz       $v0, 0x80199d4c
0x80199d40: nop        
0x80199d44: jal        0x801990f0
0x80199d48: nop        
0x80199d4c: lw         $ra, 0x38($sp)
0x80199d50: addiu      $sp, $sp, 0x40
0x80199d54: jr         $ra