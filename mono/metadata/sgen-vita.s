	.syntax	unified
	.fpu	neon-fp16
	.eabi_attribute 38, 1
	.text
	.align	2
	.text
	.global	mono_sgen_arch_store_regs
	.type	mono_sgen_arch_store_regs,function
	.thumb
	.thumb_func
mono_sgen_arch_store_regs:
	.fnstart
.LFB1:
	.cfi_startproc
	.cfi_def_cfa_offset 0
.LRET_mono_sgen_arch_store_regs:
	stmia	r0!, {r0-r12}
	bx	lr
	.cfi_endproc
.LFE1:
	.size mono_sgen_arch_store_regs, . - mono_sgen_arch_store_regs
	.fnend
	.section	.data
