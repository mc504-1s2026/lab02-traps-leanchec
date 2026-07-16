#include <kernel/serial.h>
#include <kernel/panic.h>
#include <kernel/printf.h>
#include <arch/io.h>
#include <arch/csr.h>
#include <arch/plic.h>
#include <arch/spinlock.h>

#define SERIAL_BUF_SIZE 256

static struct {
	char buf[SERIAL_BUF_SIZE];
	size_t len;
	struct spinlock lock;
} dev;

static void *serial_reg(u64 reg)
{
	return (void *)((u64)SERIAL_BASE + reg);
}

void serial_init()
{
	spin_init(&dev.lock);
	dev.len = 0;

	iowrite8(0x00, serial_reg(SERIAL_IER));
	iowrite8(SERIAL_FCR_FIFO_ENABLE | SERIAL_FCR_RX_FIFO_CLEAR | SERIAL_FCR_TX_FIFO_CLEAR,
		 serial_reg(SERIAL_FCR));
	iowrite8(0x03, serial_reg(SERIAL_LCR));
	iowrite8(0x00, serial_reg(SERIAL_MCR));
	iowrite8(SERIAL_IER_ERBFI, serial_reg(SERIAL_IER));
}

void serial_irq_enable()
{
	plic_irq_set_priority(IRQ_SERIAL, 1);
	plic_hart_set_threshold(0, 0);
	plic_hart_enable_irq(0, IRQ_SERIAL);
	csr_set(CSR_SIE, CSR_SIE_SEIE);
}

void serial_irq_disable()
{
	csr_clear(CSR_SIE, CSR_SIE_SEIE);
}

void serial_irq()
{
	spin_lock(&dev.lock);
	while (ioread8(serial_reg(SERIAL_LSR)) & SERIAL_LSR_DTR) {
		char c = (char)ioread8(serial_reg(SERIAL_RBR));
		if (dev.len < SERIAL_BUF_SIZE) {
			dev.buf[dev.len++] = c;
		}
	}
	spin_unlock(&dev.lock);
}

size_t serial_read(char *buf)
{
	u64 flags = spin_lock_irqsave(&dev.lock);
	size_t n = dev.len;
	for (size_t i = 0; i < n; i++) {
		buf[i] = dev.buf[i];
	}
	dev.len = 0;
	spin_unlock_irqrestore(&dev.lock, flags);
	return n;
}

void serial_putc(char c)
{
	while (!(ioread8(serial_reg(SERIAL_LSR)) & SERIAL_LSR_THRE))
		;
	iowrite8((u8)c, serial_reg(SERIAL_THR));
}

void serial_puts(char *str)
{
	while (*str) {
		serial_putc(*str);
		str++;
	}
}
