# Assignment 9 — Page Tables, Lazy Allocation, and Page Replacement

**Name:** R. Rahul  
**Roll Number:** 2401MC49  
**Course:** Operating Systems  
**Assignment:** Assignment 9 (Page Tables, Lazy Allocation, and Page Replacement)

**Base System:** xv6-riscv (Sv39, three-level page tables)

The implementation adapts concepts from the x86 assignment to the RISC-V architecture. The `walkpgdir` functionality is replaced with a recursive Sv39 page-table traversal, `rcr2()` with `r_stval()`, and `T_PGFLT` with page-fault causes 13 and 15. The x86 `P` flag is replaced by `PTE_V`, and only valid user leaf entries (`PTE_U`) are printed, excluding kernel mappings. All kernel modifications are marked with comments `// A9-Q1` or `// A9-Q2`.

## Q1 — vmprint()

**Implementation Details:**

- Added `vmprint()` and the recursive helper `vmprint_walk()` in `kernel/vm.c`.
- Printed the virtual address (VA), physical address (PA, using `PTE2PA`), and permission flags (P/W/U) for every valid user leaf PTE.
- Counted the total number of user pages and page-table pages, including the root table and all non-leaf tables.
- Connected the system call through `defs.h`, `sysproc.c`, `syscall.h`, `syscall.c`, `usys.pl`, and `user.h`.
- Created `tests/pttest.c` to display the page-table structure at startup and again after calling `sbrk(4*4096)` and accessing all four newly allocated pages.

**Results:**

- Initially, the process contained 3 user pages at virtual addresses `0x0`, `0x1000`, and `0x3000`, with 5 page-table pages.
- After allocating and accessing four additional pages, the process contained 7 user pages, with the new pages occupying `0x4000` through `0x7000`.
- The page-table page count remained unchanged at 5.

**Status:** PASS — Four additional user pages were successfully mapped and verified.

## Q2 — Lazy Allocation

**Implementation Details:**

- Added `page_fault_count` to `struct proc` in `proc.h` and initialized it to zero in `allocproc()` in `proc.c`.
- Modified `sys_sbrk()` in `sysproc.c` so that positive increments increase the process size without immediately allocating physical memory. Negative increments continue to free memory.
- Added `sys_getfaults()` to return the current process's page-fault counter.
- Updated `uvmcopy()` and `uvmunmap()` in `vm.c` to skip unmapped pages using `continue` instead of triggering `panic`.
- Modified `trap.c` to handle load and store page faults (scause 13 and 15). When the faulting address is below the process size, the kernel allocates a physical page using `kalloc()`, clears it with `memset()`, maps it with `mappages()` using `PTE_R | PTE_W | PTE_U`, and increments `page_fault_count`.
- Invalid faults print the process ID and faulting address, then terminate only the affected process.
- Implemented `tests/faulttest.c` to allocate 32 pages using `sbrk(32*4096)`, write to five pages, and verify the fault-count difference. A child process also attempts to access an address beyond its allocated size.

**Results:**

- The five writes produced exactly five page faults: **PASS**.
- The child process accessed an invalid address, triggering the message `pid 4: fault at 0x26000 (sz=0x25000), killed`.
- The invalid access terminated the child process while allowing the parent process to continue: **PASS**.

**Status:** PASS — Lazy allocation, page-fault counting, and invalid-access handling were verified.

## Q3 — FIFO vs. LRU Page Replacement

**Implementation Details:**

- Implemented the simulation entirely in user space in `tests/pagerepl.c`, without modifying the kernel.
- Implemented FIFO using a circular victim pointer.
- Implemented LRU using timestamps to track the most recent use of each page.
- Displayed the frames after every reference, identifying each access as a hit or a fault.
- Generated a comparison table for 3, 4, and 5 frames, including page-fault counts and hit ratios.

**Reference String:**

`7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1`

**Results:**

| Frames | FIFO Faults | LRU Faults | FIFO Hit Ratio | LRU Hit Ratio |
|---:|---:|---:|---:|---:|
| 3 | 15 | 12 | 25% | 40% |
| 4 | 10 | 8 | 50% | 60% |
| 5 | 9 | 7 | 55% | 65% |

LRU produced fewer page faults than FIFO for all three frame configurations. The results demonstrate the effectiveness of using recent access history to select replacement pages.

**Status:** PASS — Both algorithms were simulated and compared successfully.

## Project Contents

- **`diffs/`**
  - `vm.c.diff`
  - `trap.c.diff`
  - `proc.diff`
  - `sysproc.c.diff`
  - `syscall_wiring.diff`

- **`tests/`**
  - `pttest.c`
  - `faulttest.c`
  - `pagerepl.c`
  - `run_tests.sh`

- **`screenshots/`**
  - `q1_pttest.png`
  - `q2_faulttest.png`
  - `q3_pagerepl_frames.png`
  - `q3_pagerepl_table.png`

## Conclusion

Assignment 9 covers three important operating-system concepts: page-table inspection, demand-based memory allocation, and page-replacement algorithms. The `vmprint()` implementation displays user page mappings and page-table statistics. Lazy allocation reduces immediate memory allocation by creating pages only when they are accessed, while invalid page faults are handled by terminating the offending process. Finally, the FIFO and LRU simulations show that LRU achieves fewer page faults for the tested reference string and frame counts.