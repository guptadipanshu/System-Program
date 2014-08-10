This code implements scheduling algoriths like 
round robin
PSJF
SJF
FCFS
PRI
round robin with variable quantums etc
Uses a priorty queue to schedule tasks on.
Claculate average wait time,turnaround time etc
To run-

make clean
make
./simulator -c <cores> -s <scheme> <input file>

For example:
./simulator -c 2 -s fcfs examples/proc1.csv
The acceptable values for scheme are:
FCFS
SJF
PSFJ
PRI
PPRI
RR#, where # indicates any numeric value

