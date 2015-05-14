	.file	"common.c"
	.section	.text.unlikely,"ax",@progbits
.LCOLDB0:
	.text
.LHOTB0:
	.globl	outb
	.type	outb, @function
outb:
.LFB1:
	.cfi_startproc
	movl	8(%esp), %eax
	movl	4(%esp), %edx
#APP
# 11 "common.c" 1
	outb %al, %dx
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE1:
	.size	outb, .-outb
	.section	.text.unlikely
.LCOLDE0:
	.text
.LHOTE0:
	.section	.text.unlikely
.LCOLDB1:
	.text
.LHOTB1:
	.globl	inb
	.type	inb, @function
inb:
.LFB2:
	.cfi_startproc
	movl	4(%esp), %edx
#APP
# 17 "common.c" 1
	inb %dx, %al
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE2:
	.size	inb, .-inb
	.section	.text.unlikely
.LCOLDE1:
	.text
.LHOTE1:
	.section	.text.unlikely
.LCOLDB2:
	.text
.LHOTB2:
	.globl	inw
	.type	inw, @function
inw:
.LFB3:
	.cfi_startproc
	movl	4(%esp), %edx
#APP
# 24 "common.c" 1
	inw %dx, %ax
# 0 "" 2
#NO_APP
	ret
	.cfi_endproc
.LFE3:
	.size	inw, .-inw
	.section	.text.unlikely
.LCOLDE2:
	.text
.LHOTE2:
	.section	.text.unlikely
.LCOLDB3:
	.text
.LHOTB3:
	.globl	mymemcpy
	.type	mymemcpy, @function
mymemcpy:
.LFB4:
	.cfi_startproc
	pushl	%ebx
	.cfi_def_cfa_offset 8
	.cfi_offset 3, -8
	movl	8(%esp), %edx
	movl	12(%esp), %ecx
	movl	16(%esp), %eax
.L5:
	decl	%eax
	cmpl	$-1, %eax
	je	.L9
	movb	(%ecx,%eax), %bl
	movb	%bl, (%edx,%eax)
	jmp	.L5
.L9:
	popl	%ebx
	.cfi_restore 3
	.cfi_def_cfa_offset 4
	ret	$12
	.cfi_endproc
.LFE4:
	.size	mymemcpy, .-mymemcpy
	.section	.text.unlikely
.LCOLDE3:
	.text
.LHOTE3:
	.section	.text.unlikely
.LCOLDB4:
	.text
.LHOTB4:
	.globl	my_memset
	.type	my_memset, @function
my_memset:
.LFB5:
	.cfi_startproc
	movl	4(%esp), %edx
	movb	8(%esp), %cl
	xorl	%eax, %eax
.L11:
	cmpl	12(%esp), %eax
	je	.L13
	movb	%cl, (%edx,%eax)
	incl	%eax
	jmp	.L11
.L13:
	ret	$12
	.cfi_endproc
.LFE5:
	.size	my_memset, .-my_memset
	.section	.text.unlikely
.LCOLDE4:
	.text
.LHOTE4:
	.section	.text.unlikely
.LCOLDB5:
	.text
.LHOTB5:
	.globl	mybzero
	.type	mybzero, @function
mybzero:
.LFB6:
	.cfi_startproc
	movl	4(%esp), %edx
	movl	8(%esp), %eax
.L15:
	decl	%eax
	cmpl	$-1, %eax
	je	.L17
	movb	$0, (%edx,%eax)
	jmp	.L15
.L17:
	ret	$8
	.cfi_endproc
.LFE6:
	.size	mybzero, .-mybzero
	.section	.text.unlikely
.LCOLDE5:
	.text
.LHOTE5:
	.section	.text.unlikely
.LCOLDB6:
	.text
.LHOTB6:
	.globl	my_strcmp
	.type	my_strcmp, @function
my_strcmp:
.LFB7:
	.cfi_startproc
	pushl	%ebx
	.cfi_def_cfa_offset 8
	.cfi_offset 3, -8
	movl	12(%esp), %ecx
	xorl	%edx, %edx
.L19:
	movl	8(%esp), %eax
	movb	(%eax,%edx), %bl
	testb	%bl, %bl
	je	.L21
	movb	(%ecx,%edx), %al
	testb	%al, %al
	je	.L28
	cmpb	%al, %bl
	jne	.L24
	incl	%edx
	jmp	.L19
.L28:
	xorl	%eax, %eax
	jmp	.L20
.L21:
	xorl	%eax, %eax
	cmpb	$0, (%ecx,%edx)
	setne	%al
	jmp	.L26
.L24:
	movl	$1, %eax
.L20:
	cmpb	$0, (%ecx,%edx)
	jne	.L26
	movl	$1, %eax
.L26:
	popl	%ebx
	.cfi_restore 3
	.cfi_def_cfa_offset 4
	ret
	.cfi_endproc
.LFE7:
	.size	my_strcmp, .-my_strcmp
	.section	.text.unlikely
.LCOLDE6:
	.text
.LHOTE6:
	.section	.text.unlikely
.LCOLDB7:
	.text
.LHOTB7:
	.globl	my_strcpy
	.type	my_strcpy, @function
my_strcpy:
.LFB8:
	.cfi_startproc
	movl	4(%esp), %eax
	movl	8(%esp), %edx
.L30:
	incl	%edx
	movb	-1(%edx), %cl
	incl	%eax
	movb	%cl, -1(%eax)
	cmpb	$0, (%edx)
	jne	.L30
	ret
	.cfi_endproc
.LFE8:
	.size	my_strcpy, .-my_strcpy
	.section	.text.unlikely
.LCOLDE7:
	.text
.LHOTE7:
	.section	.text.unlikely
.LCOLDB8:
	.text
.LHOTB8:
	.globl	strcat
	.type	strcat, @function
strcat:
.LFB9:
	.cfi_startproc
	movl	4(%esp), %eax
	movl	8(%esp), %edx
.L33:
	cmpb	$0, (%eax)
	je	.L35
	incl	%eax
	jmp	.L33
.L35:
	incl	%edx
	movb	-1(%edx), %cl
	incl	%eax
	movb	%cl, -1(%eax)
	cmpb	$0, (%edx)
	jne	.L35
	ret
	.cfi_endproc
.LFE9:
	.size	strcat, .-strcat
	.section	.text.unlikely
.LCOLDE8:
	.text
.LHOTE8:
	.globl	g_tbl
	.data
	.align 4
	.type	g_tbl, @object
	.size	g_tbl, 4
g_tbl:
	.long	-889275714
	.ident	"GCC: (Debian 4.9.2-10) 4.9.2"
	.section	.note.GNU-stack,"",@progbits
