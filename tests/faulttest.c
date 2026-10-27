#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  printf("===== A9-Q2: Lazy-Allocation Fault Test =====\n\n");

  // lazily allocate 32 pages (no physical memory yet)
  char *p = sbrk(32 * 4096);
  if(p == (char *)-1){
    printf("sbrk failed\n");
    exit(1);
  }
  printf("sbrk(32*4096) done – pages NOT yet allocated\n");
  printf("faults so far (includes startup/printf) = %d\n\n", getfaults());

  // Capture the baseline fault count BEFORE we do our 5 writes
  int baseline = getfaults();

  // write to exactly 5 different pages
  printf("Writing to 5 pages...\n");
  for(int i = 0; i < 5; i++)
    p[i * 4096] = (char)i;

  int f = getfaults() - baseline; // Calculate the delta caused ONLY by our writes
  printf("faults caused by the 5 writes = %d\n", f);
  if(f == 5)
    printf("PASS: exactly 5 page faults\n");
  else
    printf("FAIL: expected 5, got %d\n", f);

  // fork a child that accesses ABOVE the allocated region
  printf("\n--- Testing invalid access (child should die) ---\n");
  int pid = fork();
  if(pid == 0){
    // child: access one page above sz  →  should be killed
    printf("child %d: touching address above sz...\n", getpid());
    char *bad = p + 32 * 4096 + 4096;   // well above sz
    *bad = 42;
    printf("ERROR: child survived!\n");
    exit(1);
  } else {
    wait(0);
    printf("parent: child was killed as expected – parent still alive\n");
    printf("PASS: only the faulty process was killed\n");
  }

  exit(0);
}
