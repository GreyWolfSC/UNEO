# SJIS(CP932) -> UNICODE �ϊ��e�[�u���w��	by Rudolph

	.rodata

	.globl sjis2unicode_tbl

	.balign 32

sjis2unicode_tbl:
	.incbin "../source/language/sjis2unicode.tbl"

