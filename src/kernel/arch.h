
#ifndef ARCH_H
#define ARCH_H

void arch_init(void);
char *arch_identify(void); // return a static string of information
void cli(void); // disable interrupts, or any other thing that may cause CPU state to change other than reset
void sti(void); // enable interrupts, or any other thing that may cause CPU state to change
void pause(void);
int arch_temp_mc(void);

#endif
