/* System call, according to table
   push parameters : %esp + 4 : 1ere paramètre
                  : %esp + 8 : 2ème paramètre
  call int $49

 1ere paramètre: %ebx
 2eme paramètre: %ecx
 3eme paramètre: %edx
 4eme paramètre: %esi
 5eme paramètre: %edi
 6eme paramètre: %ebp
*/

/*#include "syscall_table.h"*/

.text
.globl start
start:

    pushl %ebx
    pushl %ecx
    pushl %edx
    pushl %esi
    movl $0,%eax
    movl 20(%esp), %ebx
    movl 24(%esp), %ecx
    movl 28(%esp), %edx
    movl 32(%esp), %esi
    int $49
    pop %esi
    pop %edx
    pop %ecx
    pop %ebx

    ret



.globl getpid
getpid:

    movl $1,%eax
    int $49

    ret


.globl getprio
getprio:

    pushl %ebx
    movl $2,%eax
    movl 8(%esp),%ebx
    int $49
    pop %ebx

    ret


.globl chprio
chprio:

    pushl %ebx
    pushl %ecx
    movl $3,%eax
    movl 12(%esp),%ebx
    movl 16(%esp),%ecx
    int $49
    pop %ecx
    pop %ebx

    ret


.globl kill
kill:

    pushl %ebx
    movl $4,%eax
    movl 8(%esp),%ebx
    int $49
    pop %ebx

    ret


.globl waitpid
waitpid:

    pushl %ebx
    pushl %ecx
    movl $5,%eax
    movl 12(%esp), %ebx
    movl 16(%esp), %ecx
    int $49
    pop %ecx
    pop %ebx

    ret


.globl exit
exit:

    pushl %ebx
    movl $6,%eax
    movl  8(%esp), %ebx
    int $49
    pop %ebx

    ret


.globl cons_write
cons_write:

    pushl %ebx
    pushl %ecx
    movl $7,%eax
    movl 12(%esp), %ebx
    movl 16(%esp), %ecx
    int $49
    pop %ecx
    pop %ebx

    ret

.globl cons_read
cons_read:

    pushl %ebx
    pushl %ecx
    movl $8,%eax
    movl 12(%esp), %ebx
    movl 16(%esp), %ecx
    int $49
    pop %ecx
    pop %ebx

    ret

.globl cons_echo
cons_echo:

    pushl %ebx
    movl $9,%eax
    movl 8(%esp), %ebx
    int $49
    pop %ecx

    ret

.globl efface_ecran
efface_ecran:

    movl $31,%eax
    int $49
    ret

.globl print_all_processes
print_all_processes:

    movl $32,%eax
    int $49
    ret

.globl pcount
pcount:

    pushl %ebx
    pushl %ecx
    movl $18,%eax
    movl 12(%esp), %ebx
    movl 16(%esp), %ecx
    int $49
    pop %ecx
    pop %ebx

    ret



.globl pcreate
pcreate:

    pushl %ebx
    movl $19,%eax
    movl  8(%esp), %ebx
    int $49
    pop %ebx

    ret



.globl pdelete
pdelete:

    pushl %ebx
    movl $20,%eax
    movl  8(%esp), %ebx
    int $49
    pop %ebx

    ret


.globl preceive
preceive:

    pushl %ebx
    pushl %ecx
    movl $21,%eax
    movl 12(%esp), %ebx
    movl 16(%esp), %ecx
    int $49
    pop %ecx
    pop %ebx

    ret



.globl preset
preset:

    pushl %ebx
    movl $22,%eax
    movl  8(%esp), %ebx
    int $49
    pop %ebx

    ret

.globl psend
psend:

    pushl %ebx
    pushl %ecx
    movl $23,%eax
    movl 12(%esp), %ebx
    movl 16(%esp), %ecx
    int $49
    pop %ecx
    pop %ebx

    ret


.globl clock_settings
clock_settings:

    pushl %ebx
    pushl %ecx
    movl $24,%eax
    movl 12(%esp), %ebx
    movl 16(%esp), %ecx
    int $49
    pop %ecx
    pop %ebx

    ret


.globl current_clock
current_clock:

    movl $25,%eax
    int $49

    ret


.globl wait_clock
wait_clock:

    pushl %ebx
    movl $26,%eax
    movl  8(%esp), %ebx
    int $49
    pop %ebx

    ret


.globl shm_create
shm_create:

    pushl %ebx
    movl $28,%eax
    movl  8(%esp), %ebx
    int $49
    pop %ebx

    ret

.globl shm_acquire
shm_acquire:

    pushl %ebx
    movl $29,%eax
    movl  8(%esp), %ebx
    int $49
    pop %ebx

    ret


.globl shm_release
shm_release:

    pushl %ebx
    movl $30,%eax
    movl  8(%esp), %ebx
    int $49
    pop %ebx

    ret
