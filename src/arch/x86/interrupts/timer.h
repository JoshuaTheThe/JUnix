
#ifndef PIT_H
#define PIT_H

#define PIT_FREQUENCY 1193180
#define PIT_CHANNEL0 0x40
#define PIT_COMMAND 0x43
#define PIT_STATUS 0x61
#define PIC1_DATA 0x21
#define IRQ_TIMER 0


void kdelay(unsigned long ticks);
void timer_init(unsigned int targetFreq);

void disable_timer(void);
void enable_timer(void);

#endif
