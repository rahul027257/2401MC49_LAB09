#include "kernel/types.h"
#include "user/user.h"

#define MAXF 10
#define MAXR 100

int refs[MAXR];
int nrefs;
int fr[MAXF];
int nf;

static void
print_frames(int cnt)
{
  int j;
  printf(" frames [");
  for(j = 0; j < cnt; j++){
    if(j) printf(" ");
    printf("%d", fr[j]);
  }
  printf("]\n");
}

int
fifo(int verbose)
{
  int faults = 0, cnt = 0, head = 0, r, j, hit;
  for(j = 0; j < nf; j++) fr[j] = -1;
  for(r = 0; r < nrefs; r++){
    int pg = refs[r];
    hit = 0;
    for(j = 0; j < cnt; j++)
      if(fr[j] == pg){ hit = 1; break; }
    if(!hit){
      faults++;
      if(cnt < nf) fr[cnt++] = pg;
      else { fr[head] = pg; head = (head + 1) % nf; }
    }
    if(verbose){
      printf("  ref %d : %s", pg, hit ? "HIT   " : "FAULT ");
      print_frames(cnt);
    }
  }
  return faults;
}

int
lru(int verbose)
{
  int faults = 0, cnt = 0, r, j, idx;
  int last[MAXF];
  for(j = 0; j < nf; j++){ fr[j] = -1; last[j] = -1; }
  for(r = 0; r < nrefs; r++){
    int pg = refs[r];
    idx = -1;
    for(j = 0; j < cnt; j++)
      if(fr[j] == pg){ idx = j; break; }
    if(idx >= 0){
      last[idx] = r;
    } else {
      faults++;
      if(cnt < nf){ fr[cnt] = pg; last[cnt] = r; cnt++; }
      else {
        int v = 0;
        for(j = 1; j < cnt; j++)
          if(last[j] < last[v]) v = j;
        fr[v] = pg; last[v] = r;
      }
    }
    if(verbose){
      printf("  ref %d : %s", pg, idx >= 0 ? "HIT   " : "FAULT ");
      print_frames(cnt);
    }
  }
  return faults;
}

int
main(void)
{
  int def[] = {7,0,1,2,0,3,0,4,2,3,0,3,2,1,2,0,1,7,0,1};
  int f, a, b, i;

  nrefs = 20;
  for(i = 0; i < nrefs; i++) refs[i] = def[i];

  printf("===== A9-Q3: Page Replacement Algorithms =====\n");
  printf("Reference string: ");
  for(i = 0; i < nrefs; i++) printf("%d ", refs[i]);
  printf("\n");

  nf = 3;
  printf("\n--- FIFO (3 frames, expect 15 faults) ---\n");
  a = fifo(1);
  printf("  => FIFO faults = %d\n", a);

  printf("\n--- LRU (3 frames, expect 12 faults) ---\n");
  b = lru(1);
  printf("  => LRU faults = %d\n", b);

  printf("\n--- Comparison Table (faults / hit ratio) ---\n");
  printf("frames | FIFO faults | FIFO hit ratio | LRU faults | LRU hit ratio\n");
  for(f = 3; f <= 5; f++){
    nf = f;
    a = fifo(0);
    b = lru(0);
    printf("  %d    |     %d       | %d/%d (%d%%)   |     %d      | %d/%d (%d%%)\n",
           f, a, nrefs-a, nrefs, (nrefs-a)*100/nrefs,
              b, nrefs-b, nrefs, (nrefs-b)*100/nrefs);
  }
  exit(0);
}
