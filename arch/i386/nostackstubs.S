	.macro nos_memcpy to, from, size
nos_memcpy:
        movl \to, %edx
        movl \from, %ecx
        movl \size, %eax
1:      
	decl	%eax
	cmpl	$-1, %eax
	je	1f
	movb	(%ecx,%eax), %bl
	movb	%bl, (%edx,%eax)
	jmp	1b
1:      
        .endm

	.macro nos_memset area, size
       	xorl	%eax, %eax
1:      
	cmpl	12(%esp), %eax
	je	1f
	movb	%cl, (%edx,%eax)
	incl	%eax
	jmp	1b
1:
        .endm
