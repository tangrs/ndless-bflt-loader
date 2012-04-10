/* Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/ */

__base: .global __base
_start: .global _start
lib_main: .global lib_main
    stmfd sp!, {r4-r11,lr}
	str  sp, __crt0_savedsp
	bl   main

__crt0exit: .global __crt0exit
	ldmfd sp!, {r4-r11,pc}

__crt0_savedsp: .global __crt0_savedsp
	.long 0
