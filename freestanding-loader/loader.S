/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

/* This is a free standing loader to load basic bFLT binaries without
   a loader - can be useful in some situations */


/* If this is defined, basic sanity checks arent done.
   This results in a smaller (by ~52bytes) but more fragile binary.

   Its up to the developer to ensure the bFLT binary is sane
   before loading */
//#define LIGHT_LOADER

_start: .global _start
    .asciz "PRG"
    adr r12, bflt_executable_start

#ifndef LIGHT_LOADER
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
#endif

    /* lets begin */
    push {r0, r1, r4, r5, lr}

    /* check if bss is longer than end of file */

    /* mov lr to r2 in case we error out in bss checking */
    mov r2, lr

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

    mov r1, #4
    mla r1, r0, r1, r3 /* get end of file offset */

    /* assume reloc_start + reloc_count * 4 = end of file */
    cmp r4, r1
    /* error out if bss is too large (lr is still saved in r2) */
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

    mov r1, #0
    ldr r0,[r3]
    str r1,[r3] /* zero out after we finish and let it double as bss */
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


#ifndef LIGHT_LOADER
bflt_magic: .word 0x544c4662
#endif

bflt_executable_start: