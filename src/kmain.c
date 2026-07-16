#include <kernel/printf.h>
#include <kernel/mm.h>
#include <arch/timer.h>
#include <kernel/trap.h>
#include <kernel/serial.h>
#include <kernel/string.h>

static u64 boot_time;

static void shell_exec(char *cmd, size_t len)
{
	if (len == 0)
		return;

	cmd[len] = '\0';

	if (strcmp(cmd, "uptime") == 0) {
		u64 now = timer_read();
		u64 secs = (now - boot_time) / TIMER_FREQ;
		char buf[32];
		snprintf(buf, sizeof(buf), "%lus\n", secs);
		serial_puts(buf);
	} else if (strncmp(cmd, "echo ", 5) == 0) {
		serial_puts(cmd + 5);
		serial_putc('\n');
	} else if (strncmp(cmd, "alarm ", 6) == 0) {
		u64 secs = strtou64(cmd + 6, 10);
		timer_set_alarm(secs);
	}
}

extern int _hartid[];
void kmain()
{
	printk_set_level(LOG_DEBUG);
	info("entered S-mode\n");
	info("booting on hart %d\n", _hartid[0]);
	info("setting up virtual memory...\n");
	vm_init();

	info("enabling traps...\n");
	trap_setup();
	info("enabling timer...\n");
	timer_irq_enable();
	info("enabling serial...\n");
	serial_init();
	serial_irq_enable();

	boot_time = timer_read();

	char line[256];
	size_t line_len = 0;
	char tmp[256];

	serial_puts("> ");

	while (1) {
		size_t n = serial_read(tmp);
		for (size_t i = 0; i < n; i++) {
			char c = tmp[i];
			if (c == '\r') {
				serial_puts("\n");
				shell_exec(line, line_len);
				line_len = 0;
				serial_puts("> ");
			} else if (c == 127 || c == '\b') {
				if (line_len > 0) {
					line_len--;
					serial_puts("\b \b");
				}
			} else {
				if (line_len < sizeof(line) - 1) {
					line[line_len++] = c;
					serial_putc(c);
				}
			}
		}
	}
}
