
#ifndef PIT_H
#define PIT_H

#define PIT_FREQUENCY 1193180
#define PIT_CHANNEL0 0x40
#define PIT_COMMAND 0x43
#define PIT_STATUS 0x61

void kdelay(unsigned long ticks);
void timer_init(unsigned int targetFreq);

#endif
