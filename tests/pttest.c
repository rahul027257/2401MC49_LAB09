#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  printf("===== A9-Q1: Page-Table Print Test =====\n\n");

  printf("--- (a) Initial page table ---\n");
  vmprint();

  // allocate 4 pages via sbrk
  char *p = sbrk(4 * 4096);
  if(p == (char *)-1){
    printf("sbrk failed\n");
    exit(1);
  }
  // touch every page so lazy-alloc maps them
  for(int i = 0; i < 4; i++)
    p[i * 4096] = (char)i;

  printf("\n--- (b) After sbrk(4*4096) + touching 4 pages ---\n");
  vmprint();

  printf("\nDone – compare the two dumps; exactly 4 new user pages\n");
  exit(0);
}
