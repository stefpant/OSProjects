# Inverted Page Table Simulation

## Compile and Run
* make
* ./main k q [max]
* make clean

## Description
Simulation of Memory Management System based on Trace Reference!
Simulator using Inverted Page Table as Virtual Memory with
FWF(Flush When Full) as replacement algorithm!
There are 2 processes reading 2 trace files and sometimes they
come across Page Faults(PF:Page not already in Page Table...insert).
When any process find k+1(k:first argument) PF flushes the page table!
Processes reading reference traces alternately after q(2nd arg)
repetitions(reads)!
Optional 3rd arg: max reference traces to be read from each process.If
not given then processes read whole trace file(1M trace references).

### At the end Simulator shows for every process results about:
* Total Page Faults
* Total Write Backs in Disc("Dirty" pages after flush)
* Total Flushes
