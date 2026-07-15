# this is what will be loaded into the stvec CSR; the
# trap handler address must be 4-byte aligned:
# https://docs.riscv.org/reference/isa/priv/supervisor.html#12-1-1-2-supervisor-trap-vector-base-address-stvec-register
.globl trap_entry
.align 4
trap_entry:

    # your code here; save registers to the stack

    addi sp, sp, -128

    sw ra, 0(sp)

    sw t0, 4(sp)
    sw t1, 8(sp)
    sw t2, 12(sp)
    sw t3, 16(sp)
    sw t4, 20(sp)
    sw t5, 24(sp)
    sw t6, 28(sp)

    sw a0, 32(sp)
    sw a1, 36(sp)
    sw a2, 40(sp)
    sw a3, 44(sp)
    sw a4, 48(sp)
    sw a5, 52(sp)
    sw a6, 56(sp)
    sw a7, 60(sp)

    sw s0, 64(sp)
    sw s1, 68(sp)
    sw s2, 72(sp)
    sw s3, 76(sp)
    sw s4, 80(sp)
    sw s5, 84(sp)
    sw s6, 88(sp)
    sw s7, 92(sp)
    sw s8, 96(sp)
    sw s9, 100(sp)
    sw s10, 104(sp)
    sw s11, 108(sp)


    call handle_trap

    # your code here; restore registers

    lw ra, 0(sp)

    lw t0, 4(sp)
    lw t1, 8(sp)
    lw t2, 12(sp)
    lw t3, 16(sp)
    lw t4, 20(sp)
    lw t5, 24(sp)
    lw t6, 28(sp)

    lw a0, 32(sp)
    lw a1, 36(sp)
    lw a2, 40(sp)
    lw a3, 44(sp)
    lw a4, 48(sp)
    lw a5, 52(sp)
    lw a6, 56(sp)
    lw a7, 60(sp)

    lw s0, 64(sp)
    lw s1, 68(sp)
    lw s2, 72(sp)
    lw s3, 76(sp)
    lw s4, 80(sp)
    lw s5, 84(sp)
    lw s6, 88(sp)
    lw s7, 92(sp)
    lw s8, 96(sp)
    lw s9, 100(sp)
    lw s10, 104(sp)
    lw s11, 108(sp)

    addi sp, sp, 128

    sret
