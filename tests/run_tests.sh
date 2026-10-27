#!/bin/sh
echo "Build with: make clean && make qemu"
echo "Then run inside xv6 shell:"
echo "  pttest       # Q1 - Page table dump (3 -> 7 pages)"
echo "  faulttest    # Q2 - 5 lazy faults + child killed above sz"
echo "  pagerepl     # Q3 - FIFO 15 / LRU 12 + 3/4/5-frame table"
