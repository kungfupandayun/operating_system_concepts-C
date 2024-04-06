#include "sysapi.h"

int main(void * arg) {
  (void) arg;
  int res;
  int pid;
  printf("On commence...\n");
  for (int i=0; i<1000; i++) {
    printf("%d ", i);
    pid = start("simple", 4000, 192, (void*)i);
    assert(waitpid(pid, &res)==pid);
    assert(res==i);
  }
  printf("Et on a fini.\n");
  return 0;
}
