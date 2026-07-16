#include <kernel/trap.h>
#include <kernel/panic.h>
#include <kernel/printf.h>
#include <kernel/serial.h>
#include <arch/csr.h>
#include <arch/plic.h>
#include <arch/timer.h>

extern void trap_entry();

void handle_irq(u64 cause)
{
	if (cause == TRAP_TIMER_IRQ) {
		timer_irq();
	} else if (cause == TRAP_EXTERNAL_IRQ) {
		u32 irq = plic_hart_claim_irq(0);
		if (irq == IRQ_SERIAL) {
			serial_irq();
		}
		if (irq) {
			plic_hart_complete_irq(0, irq);
		}
	}
}

void handle_exception(u64 cause, u64 stval)
{
	panic("exception: cause=%lu stval=%p\n", cause, stval);
}

void trap_setup()
{
	csr_write(CSR_STVEC, (u64)trap_entry);
	csr_clear(CSR_SSTATUS, CSR_SSTATUS_SIE);
}

void handle_trap()
{
	u64 cause = csr_read(CSR_SCAUSE);
	if (cause & TRAP_IRQ_BIT) {
		handle_irq(cause);
	} else {
		u64 stval = csr_read(CSR_STVAL);
		handle_exception(cause, stval);
	}
}

void hart_irq_enable()
{
	csr_set(CSR_SSTATUS, CSR_SSTATUS_SIE);
}

u64 hart_irq_save()
{
	u64 flags = csr_read_clear(CSR_SSTATUS, CSR_SSTATUS_SIE);
	return flags & CSR_SSTATUS_SIE;
}

void hart_irq_restore(u64 flags)
{
	if (flags) {
		csr_set(CSR_SSTATUS, CSR_SSTATUS_SIE);
	}
}

void hart_irq_disable()
{
	csr_clear(CSR_SSTATUS, CSR_SSTATUS_SIE);
}
