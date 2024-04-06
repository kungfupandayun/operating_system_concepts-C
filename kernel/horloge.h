
#include <stdio.h>
#include "stdint.h"
#include <string.h>
#include "stdbool.h"

#define QUARTZ 0X1234DD
#define CLOCKFREQ 100

void tic_PIT(void);
void init_traitant_IT(int32_t num_IT, void (*traitant)(void));
void regle_freq_horloge(void);
void masque_IRQ(uint32_t num_IRQ, bool masque);
void initialisation_horloge(void);
void clock_settings(unsigned long *quartz, unsigned long *ticks);
unsigned long current_clock();
void wait_clock(unsigned long clock);
