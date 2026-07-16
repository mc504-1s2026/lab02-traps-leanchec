#include <arch/timer.h>
#include <arch/csr.h>
#include <kernel/printf.h>
#include <kernel/serial.h>

static volatile u64 alarm_target = 0;

u64 timer_read()
{
	return csr_read(CSR_TIME);
}

void timer_irq_enable()
{
	csr_set(CSR_SIE, CSR_SIE_STIE);
	csr_write(CSR_STIMECMP, (u64)-1);
	csr_set(CSR_SSTATUS, CSR_SSTATUS_SIE);
}

void timer_irq_disable()
{
	csr_clear(CSR_SIE, CSR_SIE_STIE);
}

void timer_set_alarm(u64 secs)
{
	u64 target = csr_read(CSR_TIME) + secs * TIMER_FREQ;
	alarm_target = target;
	csr_write(CSR_STIMECMP, target);
}

void timer_irq()
{
	csr_write(CSR_STIMECMP, (u64)-1);
	if (alarm_target != 0) {
		alarm_target = 0;
		serial_puts("alarm\n");
	}
}
