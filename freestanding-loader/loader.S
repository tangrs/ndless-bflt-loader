_start: .global _start
    .asciz "PRG"
    adr r12, bflt_executable_start

    /* check magic number */
    ldr r2, [r12]
    ldr r3, bflt_magic
    cmp r2, r3
    movne pc, lr /* error out if incorrect */

    /* check version */
    add r3, r12, #7
    ldrb r2, [r3] /* just get the last byte to avoid endian issues */
    cmp r2, #4
    movne pc, lr /* error out if not like */

    /* check for unsupported flags */
    add r3, r12, #9
    ldrb r2, [r3]
    and r2, r2, #0xc
    cmp r2, #0
    movne pc, lr /* error out if unsupported flags found */


    /* lets begin */
    push {r0, r1, r4, r5, lr}

    /* check if bss is longer than end of file */
    add r5, r12, #20 /* bss_end */
    ldr r0, [r5]
    bl endian_fix
    mov r4, r0

    add r5, r12, #28 /* reloc_start */
    ldr r0, [r5]
    bl endian_fix
    mov r3, r0

    add r5, r12, #32 /* reloc_count */
    ldr r0, [r5]
    bl endian_fix

    mov r2, #4
    mla r1, r0, r2, r3

    /* assume reloc_start + reloc_count * 4 = end of file */
    cmp r1, r4
    /* error out of bss is too large */
    movgt r2, lr
    bgt restore_to_r2


    /* get entry offset */
    add r1, r12, #8
    ldr r0, [r1]
    bl endian_fix
    /* absolute address of entry point */
    add r4, r0, r12

    /* begin relocating data - get reloc offset */
    add r1, r12, #28
    ldr r0, [r1]
    bl endian_fix
    /* data file offset is in r0 */
    add r3, r0, r12 /* convert to real address */

    add r2, r12, #32
    ldr r0, [r2]
    bl endian_fix
    mov r2, r0
    /* r0 = scrap
       r1 = scrap
       r2 = count
       r3 = reloc_ptr
       r4 = entry_ptr
       r5 = ptr to ptr */
    data_reloc:
    cmp r2, #0
    beq data_finish

    ldr r0,[r3]
    bl endian_fix
    add r5, r0, r4 /* absolute address of fixup */

    ldr r0,[r5]
    add r0, r0, r4 /* offset fixup */
    str r0,[r5] /* store back */

    sub r2, r2, #1
    add r3, r3, #4
    b data_reloc

    data_finish:

    /* get data start offset and relocate */
    add r1, r12, #12
    ldr r0, [r1]
    bl endian_fix
    /* data file offset is in r0 */
    add r3, r0, r12 /* convert to real address */

    /* r0 = scrap
       r1 = scrap
       r2 = scrap
       r3 = got_ptr
       r4 = real address of start of image
    */
    got_relocate_loop:
    ldr r0, [r3]
    cmp r0, #-1
    beq got_finish

    add r0, r0, r4
    str r0, [r3]
    add r3, r3, #4
    b got_relocate_loop

    got_finish:
    bkpt #1
    /* begin zeroing bss */
    add r1, r12, #28
    ldr r0, [r1]
    bl endian_fix
    /* data file offset is in r0 */
    add r3, r0, r12 /* convert to real address */

    add r2, r12, #32
    ldr r0, [r2]
    bl endian_fix
    mov r1, #0
    /* r0 = count
       r1 = zero
       r2 = scrap
       r3 = reloc_ptr
       r4 = entry_ptr
       r5 = scrap */
    bss_zero:
    cmp r0, #0
    beq bss_zero_finish
    str r1, [r3]
    sub r0, r0, #1
    add r3, r3, #4
    b bss_zero

    bss_zero_finish:

    /* everything should be fixed now */
    mov r2, r4
    restore_to_r2:
    pop {r0, r1, r4, r5, lr}
    mov pc, r2 /* entry point */


endian_fix: /* r0 = integer r1 is clobbered */
    eor r1, r0, r0, ror #16
    bic r1, r1, #0xff, 16
    mov r0, r0, ror #8
    eor r0, r0, r1, lsr #8
    mov pc, lr



bflt_magic: .word 0x544c4662

bflt_executable_start: