#include "cpu.h"
#include "ecran.h"
#include <stdio.h>
#include <string.h>
#include "segment.h"
#include "stdbool.h"
#include "stdint.h"
#include "horloge.h"
#include "process.h"


extern void traitant_IT_32(void);


uint32_t nb_interrupt=0;
uint32_t time = 0;

void tic_PIT(void)
{
  outb(0x20,0x20);
  nb_interrupt++;
  if (nb_interrupt%CLOCKFREQ == 0) {
    time++;
  }
  char c[9];
  sprintf(c,"%02d:%02d:%02d", (time/60)/60 , (time/60)%60 , time%60);
  print_top_right(c);
  if (nb_interrupt%2 == 0) {
    scheduler();
  }

}

void init_traitant_IT(int32_t num_IT, void (*traitant)(void))
{
  uint32_t * addr =(uint32_t*)(0x1000 + num_IT * 2 * 4);
  *(addr)=(((uint32_t)KERNEL_CS)<<16)|((uint32_t)traitant&0x0000FFFF);
  *(addr + 1) = ((uint32_t)traitant & 0xFFFF0000) | 0x0000EE00;

}

void regle_freq_horloge()
{
  outb(0x34, 0x43);

  outb(( QUARTZ / CLOCKFREQ) % 256, 0x40);
  outb(( QUARTZ / CLOCKFREQ) / 256, 0x40);
}

void masque_IRQ(uint32_t num_IRQ, bool masque){
  uint8_t val=inb(0x21);
  if(masque){
    val|=0x01<<num_IRQ;
  }
  else{
    val&=~(0x01<<num_IRQ);
  }
  outb(val, 0x21);
}

void initialisation_horloge()
{

  regle_freq_horloge();
  init_traitant_IT(32,traitant_IT_32);
  masque_IRQ(0, 0);
}

void clock_settings(unsigned long *quartz, unsigned long *ticks){
  *quartz = QUARTZ;
  *ticks = CLOCKFREQ;
}

unsigned long current_clock(){
    return nb_interrupt;
}

void wait_clock(unsigned long clock){
  // le proc principal n'est jamais endormi
  if(getpid() != 0){
    proc_table[getpid()]->wake_up_time= clock;
    proc_table[getpid()]->state = ASLEEP;
    scheduler();
  }
}
