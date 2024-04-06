#ifndef _TEST6_H_
#define _TEST6_H_




__asm__(
".text\n"
".globl proc6_1\n"
"proc6_1:\n"
"movl $3,%eax\n"
"ret\n"
".previous\n"
);




__asm__(
".text\n"
".globl proc6_2\n"
"proc6_2:\n"
"movl 4(%esp),%eax\n"
"pushl %eax\n"
"popl %eax\n"
"ret\n"
".previous\n"
);



__asm__(
".text\n"
".globl proc6_3\n"
"proc6_3:\n"
"movl 4(%esp),%eax\n"
"pushl %eax\n"
"popl %eax\n"
"ret\n"
".previous\n"
);

#endif
