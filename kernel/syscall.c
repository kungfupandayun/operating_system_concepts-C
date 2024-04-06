
#include "stdint.h"

extern void init_syscall_table();
extern void traitant_IT_49(void);
extern void init_traitant_IT(int32_t num_IT, void (*traitant)(void));

void init_syscalls() {
    init_syscall_table();
    init_traitant_IT(49,traitant_IT_49);
}
