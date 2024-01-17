	.file "/home/olga/study/23_fall_pract/Lama/performance/Sort.lama"

	.stabs "/home/olga/study/23_fall_pract/Lama/performance/Sort.lama",100,0,0,.Ltext

	.globl	main

	.data

string_0:	.string	"Sort.lama"

_init:	.int 0

	.section custom_data,"aw",@progbits

filler:	.fill	8, 4, 1

	.text

.Ltext:

	.stabs "data:t1=r1;0;4294967295;",128,0,0,0

# IMPORT ("Std") / 

# PUBLIC ("main") / 

# EXTERN ("Llowercase") / 

# EXTERN ("Luppercase") / 

# EXTERN ("LtagHash") / 

# EXTERN ("LflatCompare") / 

# EXTERN ("LcompareTags") / 

# EXTERN ("LkindOf") / 

# EXTERN ("Ltime") / 

# EXTERN ("Lrandom") / 

# EXTERN ("LdisableGC") / 

# EXTERN ("LenableGC") / 

# EXTERN ("Ls__Infix_37") / 

# EXTERN ("Ls__Infix_47") / 

# EXTERN ("Ls__Infix_42") / 

# EXTERN ("Ls__Infix_45") / 

# EXTERN ("Ls__Infix_43") / 

# EXTERN ("Ls__Infix_62") / 

# EXTERN ("Ls__Infix_6261") / 

# EXTERN ("Ls__Infix_60") / 

# EXTERN ("Ls__Infix_6061") / 

# EXTERN ("Ls__Infix_3361") / 

# EXTERN ("Ls__Infix_6161") / 

# EXTERN ("Ls__Infix_3838") / 

# EXTERN ("Ls__Infix_3333") / 

# EXTERN ("Ls__Infix_58") / 

# EXTERN ("Li__Infix_4343") / 

# EXTERN ("Lcompare") / 

# EXTERN ("Lwrite") / 

# EXTERN ("Lread") / 

# EXTERN ("Lfailure") / 

# EXTERN ("Lfexists") / 

# EXTERN ("Lfwrite") / 

# EXTERN ("Lfread") / 

# EXTERN ("Lfclose") / 

# EXTERN ("Lfopen") / 

# EXTERN ("Lfprintf") / 

# EXTERN ("Lprintf") / 

# EXTERN ("LmakeString") / 

# EXTERN ("Lsprintf") / 

# EXTERN ("LregexpMatch") / 

# EXTERN ("Lregexp") / 

# EXTERN ("Lsubstring") / 

# EXTERN ("LmatchSubString") / 

# EXTERN ("Lstringcat") / 

# EXTERN ("LreadLine") / 

# EXTERN ("Ltl") / 

# EXTERN ("Lhd") / 

# EXTERN ("Lsnd") / 

# EXTERN ("Lfst") / 

# EXTERN ("Lhash") / 

# EXTERN ("Lclone") / 

# EXTERN ("Llength") / 

# EXTERN ("Lstring") / 

# EXTERN ("LmakeArray") / 

# EXTERN ("LstringInt") / 

# EXTERN ("global_stderr") / 

# EXTERN ("global_stdout") / 

# EXTERN ("global_sysargs") / 

# EXTERN ("Lsystem") / 

# EXTERN ("LgetEnv") / 

# EXTERN ("Lassert") / 

# LABEL ("main") / 

main:

# BEGIN ("main", 2, 0, [], [], []) / 

	.type main, @function

	.cfi_startproc

	movl	_init,	%eax
	test	%eax,	%eax
	jz	_continue
	ret
_ERROR:

	call	Lbinoperror
	ret
_ERROR2:

	call	Lbinoperror2
	ret
_continue:

	movl	$1,	_init
	pushl	%ebp
	.cfi_def_cfa_offset	8

	.cfi_offset 5, -8

	movl	%esp,	%ebp
	.cfi_def_cfa_register	5

	subl	$Lmain_SIZE,	%esp
	movl	%esp,	%edi
	movl	$filler,	%esi
	movl	$LSmain_SIZE,	%ecx
	rep movsl	
	call	__gc_init
	pushl	12(%ebp)
	pushl	8(%ebp)
	call	set_args
	addl	$8,	%esp
# SLABEL ("L1") / 

L1:

# LINE (29) / 

	.stabn 68,0,29,.L0

.L0:

# LINE (31) / 

	.stabn 68,0,31,.L1

.L1:

# CONST (1000) / 

	movl	$2001,	%ebx
# CALL ("Lgenerate", 1, false) / 

	pushl	%ebx
	call	Lgenerate
	addl	$4,	%esp
	movl	%eax,	%ebx
# CALL ("LbubbleSort", 1, false) / 

	pushl	%ebx
	call	LbubbleSort
	addl	$4,	%esp
	movl	%eax,	%ebx
# SLABEL ("L2") / 

L2:

# END / 

	movl	%ebx,	%eax
Lmain_epilogue:

	movl	%ebp,	%esp
	popl	%ebp
	xorl	%eax,	%eax
	.cfi_restore	5

	.cfi_def_cfa	4, 4

	ret
	.cfi_endproc

	.set	Lmain_SIZE,	0

	.set	LSmain_SIZE,	0

	.size main, .-main

# LABEL ("Lgenerate") / 

Lgenerate:

# BEGIN ("Lgenerate", 1, 0, [], ["n"], [{ blab="L5"; elab="L6"; names=[]; subs=[{ blab="L8"; elab="L9"; names=[]; subs=[{ blab="L19"; elab="L20"; names=[]; subs=[]; }; { blab="L12"; elab="L13"; names=[]; subs=[]; }]; }]; }]) / 

	.type generate, @function

	.stabs "generate:F1",36,0,0,Lgenerate

	.stabs "n:p1",160,0,0,8

	.cfi_startproc

	pushl	%ebp
	.cfi_def_cfa_offset	8

	.cfi_offset 5, -8

	movl	%esp,	%ebp
	.cfi_def_cfa_register	5

	subl	$LLgenerate_SIZE,	%esp
	movl	%esp,	%edi
	movl	$filler,	%esi
	movl	$LSLgenerate_SIZE,	%ecx
	rep movsl	
# SLABEL ("L5") / 

L5:

# SLABEL ("L8") / 

L8:

# LINE (28) / 

	.stabn 68,0,28,0

	.stabn 68,0,28,.L2-Lgenerate

.L2:

# LD (Arg (0)) / 

	movl	8(%ebp),	%ebx
# CJMP ("z", "L11") / 

	sarl	%ebx
	cmpl	$0,	%ebx
	jz	L11
# SLABEL ("L12") / 

L12:

# LD (Arg (0)) / 

	movl	8(%ebp),	%ebx
# LD (Arg (0)) / 

	movl	8(%ebp),	%ecx
# CONST (1) / 

	movl	$3,	%esi
# BINOP ("-") / 

	subl	%esi,	%ecx
	orl	$0x0001,	%ecx
# CALL ("Lgenerate", 1, false) / 

	pushl	%ebx
	pushl	%ecx
	call	Lgenerate
	addl	$4,	%esp
	popl	%ebx
	movl	%eax,	%ecx
# SEXP ("cons", 2) / 

	movl	$1697575,	%esi
	pushl	%esi
	pushl	%ecx
	pushl	%ebx
	pushl	$7
	call	Bsexp
	addl	$16,	%esp
	movl	%eax,	%ebx
# SLABEL ("L13") / 

L13:

# JMP ("L7") / 

	jmp	L7
# LABEL ("L11") / 

L11:

# SLABEL ("L19") / 

L19:

# CONST (0) / 

	movl	$1,	%ebx
# SLABEL ("L20") / 

L20:

# JMP ("L7") / 

	jmp	L7
# SLABEL ("L9") / 

L9:

# LABEL ("L7") / 

L7:

# SLABEL ("L6") / 

L6:

# END / 

	movl	%ebx,	%eax
LLgenerate_epilogue:

	movl	%ebp,	%esp
	popl	%ebp
	.cfi_restore	5

	.cfi_def_cfa	4, 4

	ret
	.cfi_endproc

	.set	LLgenerate_SIZE,	0

	.set	LSLgenerate_SIZE,	0

	.size Lgenerate, .-Lgenerate

# LABEL ("LbubbleSort") / 

LbubbleSort:

# BEGIN ("LbubbleSort", 1, 0, [], ["l"], [{ blab="L21"; elab="L22"; names=[]; subs=[{ blab="L24"; elab="L25"; names=[]; subs=[]; }]; }]) / 

	.type bubbleSort, @function

	.stabs "bubbleSort:F1",36,0,0,LbubbleSort

	.stabs "l:p1",160,0,0,8

	.cfi_startproc

	pushl	%ebp
	.cfi_def_cfa_offset	8

	.cfi_offset 5, -8

	movl	%esp,	%ebp
	.cfi_def_cfa_register	5

	subl	$LLbubbleSort_SIZE,	%esp
	movl	%esp,	%edi
	movl	$filler,	%esi
	movl	$LSLbubbleSort_SIZE,	%ecx
	rep movsl	
# SLABEL ("L21") / 

L21:

# SLABEL ("L24") / 

L24:

# LINE (22) / 

	.stabn 68,0,22,0

	.stabn 68,0,22,.L3-LbubbleSort

.L3:

# LINE (24) / 

	.stabn 68,0,24,.L4-LbubbleSort

.L4:

# LD (Arg (0)) / 

	movl	8(%ebp),	%ebx
# CALL ("Lrec_7", 1, true) / 

	movl	%ebx,	8(%ebp)
	movl	%ebp,	%esp
	popl	%ebp
	jmp	Lrec_7
# SLABEL ("L25") / 

L25:

# LABEL ("L23") / 

L23:

# SLABEL ("L22") / 

L22:

# END / 

	movl	%ebx,	%eax
LLbubbleSort_epilogue:

	movl	%ebp,	%esp
	popl	%ebp
	.cfi_restore	5

	.cfi_def_cfa	4, 4

	ret
	.cfi_endproc

	.set	LLbubbleSort_SIZE,	0

	.set	LSLbubbleSort_SIZE,	0

	.size LbubbleSort, .-LbubbleSort

# LABEL ("Lrec_7") / 

Lrec_7:

# BEGIN ("Lrec_7", 1, 1, [], ["l"], [{ blab="L27"; elab="L28"; names=[]; subs=[{ blab="L30"; elab="L31"; names=[]; subs=[{ blab="L45"; elab="L46"; names=[("l", 0)]; subs=[{ blab="L47"; elab="L48"; names=[]; subs=[]; }]; }; { blab="L38"; elab="L39"; names=[("l", 0)]; subs=[{ blab="L40"; elab="L41"; names=[]; subs=[]; }]; }]; }]; }]) / 

	.type rec_7, @function

	.stabs "rec_7:F1",36,0,0,Lrec_7

	.stabs "l:p1",160,0,0,8

	.stabs "l:1",128,0,0,-4

	.stabn 192,0,0,L45-Lrec_7

	.stabn 224,0,0,L46-Lrec_7

	.stabs "l:1",128,0,0,-4

	.stabn 192,0,0,L38-Lrec_7

	.stabn 224,0,0,L39-Lrec_7

	.cfi_startproc

	pushl	%ebp
	.cfi_def_cfa_offset	8

	.cfi_offset 5, -8

	movl	%esp,	%ebp
	.cfi_def_cfa_register	5

	subl	$LLrec_7_SIZE,	%esp
	movl	%esp,	%edi
	movl	$filler,	%esi
	movl	$LSLrec_7_SIZE,	%ecx
	rep movsl	
# SLABEL ("L27") / 

L27:

# SLABEL ("L30") / 

L30:

# LINE (18) / 

	.stabn 68,0,18,0

	.stabn 68,0,18,.L5-Lrec_7

.L5:

# LD (Arg (0)) / 

	movl	8(%ebp),	%ebx
# CALL ("Linner_7", 1, false) / 

	pushl	%ebx
	call	Linner_7
	addl	$4,	%esp
	movl	%eax,	%ebx
# DUP / 

	movl	%ebx,	%ecx
# SLABEL ("L38") / 

L38:

# DUP / 

	movl	%ecx,	%esi
# ARRAY (2) / 

	movl	$5,	%edi
	pushl	%ebx
	pushl	%ecx
	pushl	%edi
	pushl	%esi
	call	Barray_patt
	addl	$8,	%esp
	popl	%ecx
	popl	%ebx
	movl	%eax,	%esi
# CJMP ("nz", "L36") / 

	sarl	%esi
	cmpl	$0,	%esi
	jnz	L36
# LABEL ("L37") / 

L37:

# DROP / 

# JMP ("L35") / 

	jmp	L35
# LABEL ("L36") / 

L36:

# DUP / 

	movl	%ecx,	%esi
# CONST (0) / 

	movl	$1,	%edi
# ELEM / 

	pushl	%ebx
	pushl	%ecx
	pushl	%edi
	pushl	%esi
	call	Belem
	addl	$8,	%esp
	popl	%ecx
	popl	%ebx
	movl	%eax,	%esi
# CONST (1) / 

	movl	$3,	%edi
# BINOP ("==") / 

	xorl	%eax,	%eax
	cmpl	%edi,	%esi
	sete	%al
	sall	%eax
	orl	$0x0001,	%eax
	movl	%eax,	%esi
# CJMP ("z", "L37") / 

	sarl	%esi
	cmpl	$0,	%esi
	jz	L37
# DUP / 

	movl	%ecx,	%esi
# CONST (1) / 

	movl	$3,	%edi
# ELEM / 

	pushl	%ebx
	pushl	%ecx
	pushl	%edi
	pushl	%esi
	call	Belem
	addl	$8,	%esp
	popl	%ecx
	popl	%ebx
	movl	%eax,	%esi
# DROP / 

# DROP / 

# DUP / 

	movl	%ebx,	%ecx
# CONST (1) / 

	movl	$3,	%esi
# ELEM / 

	pushl	%ebx
	pushl	%esi
	pushl	%ecx
	call	Belem
	addl	$8,	%esp
	popl	%ebx
	movl	%eax,	%ecx
# ST (Local (0)) / 

	movl	%ecx,	-4(%ebp)
# DROP / 

# DROP / 

# SLABEL ("L40") / 

L40:

# LINE (19) / 

	.stabn 68,0,19,.L6-Lrec_7

.L6:

# LD (Local (0)) / 

	movl	-4(%ebp),	%ebx
# CALL ("Lrec_7", 1, true) / 

	movl	%ebx,	8(%ebp)
	movl	%ebp,	%esp
	popl	%ebp
	jmp	Lrec_7
# SLABEL ("L41") / 

L41:

# JMP ("L29") / 

	jmp	L29
# SLABEL ("L39") / 

L39:

# SLABEL ("L45") / 

L45:

# LABEL ("L35") / 

L35:

# DUP / 

	movl	%ebx,	%ecx
# DUP / 

	movl	%ecx,	%esi
# ARRAY (2) / 

	movl	$5,	%edi
	pushl	%ebx
	pushl	%ecx
	pushl	%edi
	pushl	%esi
	call	Barray_patt
	addl	$8,	%esp
	popl	%ecx
	popl	%ebx
	movl	%eax,	%esi
# CJMP ("nz", "L43") / 

	sarl	%esi
	cmpl	$0,	%esi
	jnz	L43
# LABEL ("L44") / 

L44:

# DROP / 

# JMP ("L32") / 

	jmp	L32
# LABEL ("L43") / 

L43:

# DUP / 

	movl	%ecx,	%esi
# CONST (0) / 

	movl	$1,	%edi
# ELEM / 

	pushl	%ebx
	pushl	%ecx
	pushl	%edi
	pushl	%esi
	call	Belem
	addl	$8,	%esp
	popl	%ecx
	popl	%ebx
	movl	%eax,	%esi
# CONST (0) / 

	movl	$1,	%edi
# BINOP ("==") / 

	xorl	%eax,	%eax
	cmpl	%edi,	%esi
	sete	%al
	sall	%eax
	orl	$0x0001,	%eax
	movl	%eax,	%esi
# CJMP ("z", "L44") / 

	sarl	%esi
	cmpl	$0,	%esi
	jz	L44
# DUP / 

	movl	%ecx,	%esi
# CONST (1) / 

	movl	$3,	%edi
# ELEM / 

	pushl	%ebx
	pushl	%ecx
	pushl	%edi
	pushl	%esi
	call	Belem
	addl	$8,	%esp
	popl	%ecx
	popl	%ebx
	movl	%eax,	%esi
# DROP / 

# DROP / 

# DUP / 

	movl	%ebx,	%ecx
# CONST (1) / 

	movl	$3,	%esi
# ELEM / 

	pushl	%ebx
	pushl	%esi
	pushl	%ecx
	call	Belem
	addl	$8,	%esp
	popl	%ebx
	movl	%eax,	%ecx
# ST (Local (0)) / 

	movl	%ecx,	-4(%ebp)
# DROP / 

# DROP / 

# SLABEL ("L47") / 

L47:

# LINE (20) / 

	.stabn 68,0,20,.L7-Lrec_7

.L7:

# LD (Local (0)) / 

	movl	-4(%ebp),	%ebx
# SLABEL ("L48") / 

L48:

# SLABEL ("L46") / 

L46:

# JMP ("L29") / 

	jmp	L29
# LABEL ("L32") / 

L32:

# FAIL ((18, 9), true) / 

	pushl	$19
	pushl	$37
	pushl	$string_0
	pushl	%ebx
	call	Bmatch_failure
	addl	$16,	%esp
# JMP ("L29") / 

	jmp	L29
# SLABEL ("L31") / 

L31:

# LABEL ("L29") / 

L29:

# SLABEL ("L28") / 

L28:

# END / 

	movl	%ebx,	%eax
LLrec_7_epilogue:

	movl	%ebp,	%esp
	popl	%ebp
	.cfi_restore	5

	.cfi_def_cfa	4, 4

	ret
	.cfi_endproc

	.set	LLrec_7_SIZE,	4

	.set	LSLrec_7_SIZE,	1

	.size Lrec_7, .-Lrec_7

# LABEL ("Linner_7") / 

Linner_7:

# BEGIN ("Linner_7", 1, 6, [], ["l"], [{ blab="L49"; elab="L50"; names=[]; subs=[{ blab="L52"; elab="L53"; names=[]; subs=[{ blab="L97"; elab="L98"; names=[]; subs=[{ blab="L99"; elab="L100"; names=[]; subs=[]; }]; }; { blab="L61"; elab="L62"; names=[("x", 3); ("z", 2); ("y", 1); ("tl", 0)]; subs=[{ blab="L63"; elab="L64"; names=[]; subs=[{ blab="L82"; elab="L83"; names=[]; subs=[{ blab="L89"; elab="L90"; names=[("f", 5); ("z", 4)]; subs=[{ blab="L91"; elab="L92"; names=[]; subs=[]; }]; }]; }; { blab="L71"; elab="L72"; names=[]; subs=[]; }]; }]; }]; }]; }]) / 

	.type inner_7, @function

	.stabs "inner_7:F1",36,0,0,Linner_7

	.stabs "l:p1",160,0,0,8

	.stabs "x:1",128,0,0,-16

	.stabs "z:1",128,0,0,-12

	.stabs "y:1",128,0,0,-8

	.stabs "tl:1",128,0,0,-4

	.stabn 192,0,0,L61-Linner_7

	.stabs "f:1",128,0,0,-24

	.stabs "z:1",128,0,0,-20

	.stabn 192,0,0,L89-Linner_7

	.stabn 224,0,0,L90-Linner_7

	.stabn 224,0,0,L62-Linner_7

	.cfi_startproc

	pushl	%ebp
	.cfi_def_cfa_offset	8

	.cfi_offset 5, -8

	movl	%esp,	%ebp
	.cfi_def_cfa_register	5

	subl	$LLinner_7_SIZE,	%esp
	movl	%esp,	%edi
	movl	$filler,	%esi
	movl	$LSLinner_7_SIZE,	%ecx
	rep movsl	
# SLABEL ("L49") / 

L49:

# SLABEL ("L52") / 

L52:

# LINE (7) / 

	.stabn 68,0,7,0

	.stabn 68,0,7,.L8-Linner_7

.L8:

# LD (Arg (0)) / 

	movl	8(%ebp),	%ebx
# DUP / 

	movl	%ebx,	%ecx
# SLABEL ("L61") / 

L61:

# DUP / 

	movl	%ecx,	%esi
# TAG ("cons", 2) / 

	movl	$1697575,	%edi
	movl	$5,	-28(%ebp)
	pushl	%ebx
	pushl	%ecx
	pushl	-28(%ebp)
	pushl	%edi
	pushl	%esi
	call	Btag
	addl	$12,	%esp
	popl	%ecx
	popl	%ebx
	movl	%eax,	%esi
# CJMP ("nz", "L57") / 

	sarl	%esi
	cmpl	$0,	%esi
	jnz	L57
# LABEL ("L58") / 

L58:

# DROP / 

# JMP ("L56") / 

	jmp	L56
# LABEL ("L57") / 

L57:

# DUP / 

	movl	%ecx,	%esi
# CONST (0) / 

	movl	$1,	%edi
# ELEM / 

	pushl	%ebx
	pushl	%ecx
	pushl	%edi
	pushl	%esi
	call	Belem
	addl	$8,	%esp
	popl	%ecx
	popl	%ebx
	movl	%eax,	%esi
# DROP / 

# DUP / 

	movl	%ecx,	%esi
# CONST (1) / 

	movl	$3,	%edi
# ELEM / 

	pushl	%ebx
	pushl	%ecx
	pushl	%edi
	pushl	%esi
	call	Belem
	addl	$8,	%esp
	popl	%ecx
	popl	%ebx
	movl	%eax,	%esi
# DUP / 

	movl	%esi,	%edi
# TAG ("cons", 2) / 

	movl	$1697575,	-28(%ebp)
	movl	$5,	-32(%ebp)
	pushl	%ebx
	pushl	%ecx
	pushl	%esi
	pushl	-32(%ebp)
	pushl	-28(%ebp)
	pushl	%edi
	call	Btag
	addl	$12,	%esp
	popl	%esi
	popl	%ecx
	popl	%ebx
	movl	%eax,	%edi
# CJMP ("nz", "L59") / 

	sarl	%edi
	cmpl	$0,	%edi
	jnz	L59
# LABEL ("L60") / 

L60:

# DROP / 

# JMP ("L58") / 

	jmp	L58
# LABEL ("L59") / 

L59:

# DUP / 

	movl	%esi,	%edi
# CONST (0) / 

	movl	$1,	-28(%ebp)
# ELEM / 

	pushl	%ebx
	pushl	%ecx
	pushl	%esi
	pushl	-28(%ebp)
	pushl	%edi
	call	Belem
	addl	$8,	%esp
	popl	%esi
	popl	%ecx
	popl	%ebx
	movl	%eax,	%edi
# DROP / 

# DUP / 

	movl	%esi,	%edi
# CONST (1) / 

	movl	$3,	-28(%ebp)
# ELEM / 

	pushl	%ebx
	pushl	%ecx
	pushl	%esi
	pushl	-28(%ebp)
	pushl	%edi
	call	Belem
	addl	$8,	%esp
	popl	%esi
	popl	%ecx
	popl	%ebx
	movl	%eax,	%edi
# DROP / 

# DROP / 

# DROP / 

# DUP / 

	movl	%ebx,	%ecx
# CONST (0) / 

	movl	$1,	%esi
# ELEM / 

	pushl	%ebx
	pushl	%esi
	pushl	%ecx
	call	Belem
	addl	$8,	%esp
	popl	%ebx
	movl	%eax,	%ecx
# ST (Local (3)) / 

	movl	%ecx,	-16(%ebp)
# DROP / 

# DUP / 

	movl	%ebx,	%ecx
# CONST (1) / 

	movl	$3,	%esi
# ELEM / 

	pushl	%ebx
	pushl	%esi
	pushl	%ecx
	call	Belem
	addl	$8,	%esp
	popl	%ebx
	movl	%eax,	%ecx
# ST (Local (2)) / 

	movl	%ecx,	-12(%ebp)
# DROP / 

# DUP / 

	movl	%ebx,	%ecx
# CONST (1) / 

	movl	$3,	%esi
# ELEM / 

	pushl	%ebx
	pushl	%esi
	pushl	%ecx
	call	Belem
	addl	$8,	%esp
	popl	%ebx
	movl	%eax,	%ecx
# CONST (0) / 

	movl	$1,	%esi
# ELEM / 

	pushl	%ebx
	pushl	%esi
	pushl	%ecx
	call	Belem
	addl	$8,	%esp
	popl	%ebx
	movl	%eax,	%ecx
# ST (Local (1)) / 

	movl	%ecx,	-8(%ebp)
# DROP / 

# DUP / 

	movl	%ebx,	%ecx
# CONST (1) / 

	movl	$3,	%esi
# ELEM / 

	pushl	%ebx
	pushl	%esi
	pushl	%ecx
	call	Belem
	addl	$8,	%esp
	popl	%ebx
	movl	%eax,	%ecx
# CONST (1) / 

	movl	$3,	%esi
# ELEM / 

	pushl	%ebx
	pushl	%esi
	pushl	%ecx
	call	Belem
	addl	$8,	%esp
	popl	%ebx
	movl	%eax,	%ecx
# ST (Local (0)) / 

	movl	%ecx,	-4(%ebp)
# DROP / 

# DROP / 

# SLABEL ("L63") / 

L63:

# LINE (9) / 

	.stabn 68,0,9,.L9-Linner_7

.L9:

# LD (Local (3)) / 

	movl	-16(%ebp),	%ebx
# LD (Local (1)) / 

	movl	-8(%ebp),	%ecx
# CALL ("Lcompare", 2, false) / 

	pushl	%ecx
	pushl	%ebx
	call	Lcompare
	addl	$8,	%esp
	movl	%eax,	%ebx
# CONST (0) / 

	movl	$1,	%ecx
# BINOP (">") / 

	xorl	%eax,	%eax
	cmpl	%ecx,	%ebx
	setg	%al
	sall	%eax
	orl	$0x0001,	%eax
	movl	%eax,	%ebx
# CJMP ("z", "L66") / 

	sarl	%ebx
	cmpl	$0,	%ebx
	jz	L66
# SLABEL ("L71") / 

L71:

# CONST (1) / 

	movl	$3,	%ebx
# LINE (10) / 

	.stabn 68,0,10,.L10-Linner_7

.L10:

# LD (Local (1)) / 

	movl	-8(%ebp),	%ecx
# LD (Local (3)) / 

	movl	-16(%ebp),	%esi
# LD (Local (0)) / 

	movl	-4(%ebp),	%edi
# SEXP ("cons", 2) / 

	movl	$1697575,	-28(%ebp)
	pushl	%ebx
	pushl	%ecx
	pushl	-28(%ebp)
	pushl	%edi
	pushl	%esi
	pushl	$7
	call	Bsexp
	addl	$16,	%esp
	popl	%ecx
	popl	%ebx
	movl	%eax,	%esi
# CALL ("Linner_7", 1, false) / 

	pushl	%ebx
	pushl	%ecx
	pushl	%esi
	call	Linner_7
	addl	$4,	%esp
	popl	%ecx
	popl	%ebx
	movl	%eax,	%esi
# CONST (1) / 

	movl	$3,	%edi
# ELEM / 

	pushl	%ebx
	pushl	%ecx
	pushl	%edi
	pushl	%esi
	call	Belem
	addl	$8,	%esp
	popl	%ecx
	popl	%ebx
	movl	%eax,	%esi
# SEXP ("cons", 2) / 

	movl	$1697575,	%edi
	pushl	%ebx
	pushl	%edi
	pushl	%esi
	pushl	%ecx
	pushl	$7
	call	Bsexp
	addl	$16,	%esp
	popl	%ebx
	movl	%eax,	%ecx
# CALL (".array", 2, true) / 

	pushl	%ecx
	pushl	%ebx
	pushl	$5
	call	Barray
	addl	$12,	%esp
	movl	%eax,	%ebx
# SLABEL ("L72") / 

L72:

# JMP ("L51") / 

	jmp	L51
# LABEL ("L66") / 

L66:

# SLABEL ("L82") / 

L82:

# LINE (11) / 

	.stabn 68,0,11,.L11-Linner_7

.L11:

# LD (Local (2)) / 

	movl	-12(%ebp),	%ebx
# CALL ("Linner_7", 1, false) / 

	pushl	%ebx
	call	Linner_7
	addl	$4,	%esp
	movl	%eax,	%ebx
# DUP / 

	movl	%ebx,	%ecx
# SLABEL ("L89") / 

L89:

# DUP / 

	movl	%ecx,	%esi
# ARRAY (2) / 

	movl	$5,	%edi
	pushl	%ebx
	pushl	%ecx
	pushl	%edi
	pushl	%esi
	call	Barray_patt
	addl	$8,	%esp
	popl	%ecx
	popl	%ebx
	movl	%eax,	%esi
# CJMP ("nz", "L87") / 

	sarl	%esi
	cmpl	$0,	%esi
	jnz	L87
# LABEL ("L88") / 

L88:

# DROP / 

# JMP ("L84") / 

	jmp	L84
# LABEL ("L87") / 

L87:

# DUP / 

	movl	%ecx,	%esi
# CONST (0) / 

	movl	$1,	%edi
# ELEM / 

	pushl	%ebx
	pushl	%ecx
	pushl	%edi
	pushl	%esi
	call	Belem
	addl	$8,	%esp
	popl	%ecx
	popl	%ebx
	movl	%eax,	%esi
# DROP / 

# DUP / 

	movl	%ecx,	%esi
# CONST (1) / 

	movl	$3,	%edi
# ELEM / 

	pushl	%ebx
	pushl	%ecx
	pushl	%edi
	pushl	%esi
	call	Belem
	addl	$8,	%esp
	popl	%ecx
	popl	%ebx
	movl	%eax,	%esi
# DROP / 

# DROP / 

# DUP / 

	movl	%ebx,	%ecx
# CONST (0) / 

	movl	$1,	%esi
# ELEM / 

	pushl	%ebx
	pushl	%esi
	pushl	%ecx
	call	Belem
	addl	$8,	%esp
	popl	%ebx
	movl	%eax,	%ecx
# ST (Local (5)) / 

	movl	%ecx,	-24(%ebp)
# DROP / 

# DUP / 

	movl	%ebx,	%ecx
# CONST (1) / 

	movl	$3,	%esi
# ELEM / 

	pushl	%ebx
	pushl	%esi
	pushl	%ecx
	call	Belem
	addl	$8,	%esp
	popl	%ebx
	movl	%eax,	%ecx
# ST (Local (4)) / 

	movl	%ecx,	-20(%ebp)
# DROP / 

# DROP / 

# SLABEL ("L91") / 

L91:

# LD (Local (5)) / 

	movl	-24(%ebp),	%ebx
# LD (Local (3)) / 

	movl	-16(%ebp),	%ecx
# LD (Local (4)) / 

	movl	-20(%ebp),	%esi
# SEXP ("cons", 2) / 

	movl	$1697575,	%edi
	pushl	%ebx
	pushl	%edi
	pushl	%esi
	pushl	%ecx
	pushl	$7
	call	Bsexp
	addl	$16,	%esp
	popl	%ebx
	movl	%eax,	%ecx
# CALL (".array", 2, true) / 

	pushl	%ecx
	pushl	%ebx
	pushl	$5
	call	Barray
	addl	$12,	%esp
	movl	%eax,	%ebx
# SLABEL ("L92") / 

L92:

# SLABEL ("L90") / 

L90:

# JMP ("L51") / 

	jmp	L51
# LABEL ("L84") / 

L84:

# FAIL ((11, 17), true) / 

	pushl	$35
	pushl	$23
	pushl	$string_0
	pushl	%ebx
	call	Bmatch_failure
	addl	$16,	%esp
# JMP ("L51") / 

	jmp	L51
# SLABEL ("L83") / 

L83:

# SLABEL ("L64") / 

L64:

# JMP ("L51") / 

# SLABEL ("L62") / 

L62:

# SLABEL ("L97") / 

L97:

# LABEL ("L56") / 

L56:

# DUP / 

	movl	%ebx,	%ecx
# DROP / 

# DROP / 

# SLABEL ("L99") / 

L99:

# CONST (0) / 

	movl	$1,	%ebx
# LINE (13) / 

	.stabn 68,0,13,.L12-Linner_7

.L12:

# LD (Arg (0)) / 

	movl	8(%ebp),	%ecx
# CALL (".array", 2, true) / 

	pushl	%ecx
	pushl	%ebx
	pushl	$5
	call	Barray
	addl	$12,	%esp
	movl	%eax,	%ebx
# SLABEL ("L100") / 

L100:

# SLABEL ("L98") / 

L98:

# JMP ("L51") / 

	jmp	L51
# SLABEL ("L53") / 

L53:

# LABEL ("L51") / 

L51:

# SLABEL ("L50") / 

L50:

# END / 

	movl	%ebx,	%eax
LLinner_7_epilogue:

	movl	%ebp,	%esp
	popl	%ebp
	.cfi_restore	5

	.cfi_def_cfa	4, 4

	ret
	.cfi_endproc

	.set	LLinner_7_SIZE,	32

	.set	LSLinner_7_SIZE,	8

	.size Linner_7, .-Linner_7

# LABEL ("Lcompare") / 

Lcompare:

# BEGIN ("Lcompare", 2, 0, [], ["x"; "y"], [{ blab="L103"; elab="L104"; names=[]; subs=[{ blab="L106"; elab="L107"; names=[]; subs=[]; }]; }]) / 

	.type compare, @function

	.stabs "compare:F1",36,0,0,Lcompare

	.stabs "x:p1",160,0,0,8

	.stabs "y:p1",160,0,0,12

	.cfi_startproc

	pushl	%ebp
	.cfi_def_cfa_offset	8

	.cfi_offset 5, -8

	movl	%esp,	%ebp
	.cfi_def_cfa_register	5

	subl	$LLcompare_SIZE,	%esp
	movl	%esp,	%edi
	movl	$filler,	%esi
	movl	$LSLcompare_SIZE,	%ecx
	rep movsl	
# SLABEL ("L103") / 

L103:

# SLABEL ("L106") / 

L106:

# LINE (1) / 

	.stabn 68,0,1,0

	.stabn 68,0,1,.L13-Lcompare

.L13:

# LD (Arg (0)) / 

	movl	8(%ebp),	%ebx
# LINE (2) / 

	.stabn 68,0,2,.L14-Lcompare

.L14:

# LD (Arg (1)) / 

	movl	12(%ebp),	%ecx
# BINOP ("-") / 

	subl	%ecx,	%ebx
	orl	$0x0001,	%ebx
# SLABEL ("L107") / 

L107:

# LABEL ("L105") / 

L105:

# SLABEL ("L104") / 

L104:

# END / 

	movl	%ebx,	%eax
LLcompare_epilogue:

	movl	%ebp,	%esp
	popl	%ebp
	.cfi_restore	5

	.cfi_def_cfa	4, 4

	ret
	.cfi_endproc

	.set	LLcompare_SIZE,	0

	.set	LSLcompare_SIZE,	0

	.size Lcompare, .-Lcompare

