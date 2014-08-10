This code does the following things-
Create the map() processes. System call: fork()
Read the result of the map() processes over a pipe or fifo. System call: pipe() or fifo()
Process the results of each process using only a single thread, by using I/O multiplexing. System call: epoll()
Give the current set of completed results back to the user at any time, even if some of the map() processes are not yet finished.

RUN commands-
make clean
make
./test# where #is no between 1-5
