#ifndef _TEST16_MSG_H_
#define _TEST16_MSG_H_

struct tst16 {
    int count;
    int fid;
};

void test_it(void)
{
#ifdef microblaze
        int status, mstatus;
        __asm__ volatile("mfs %0,rmsr; ori %1,%0,2; mts rmsr,%1; nop; nop; mts rmsr,%0":"=r" (status), "=r" (mstatus));
#else
        __asm__ volatile("pushfl; testl $0x200,(%%esp); jnz 0f; sti; nop; cli; 0: addl $4,%%esp\n":::"memory");
#endif
}

#endif /* _TEST16_MSG_H_ */
