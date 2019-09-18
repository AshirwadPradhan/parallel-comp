## Repo for various Parallel Programs
***

### 1. Seive of Eratothenes
    The sequential version is improved using block decomposition parallel paradigm.
    Time Complexity : O(n ln ln n)/p
            p: Number of processes 
    
    Benchmarks: [On Intel i5 7200U 2 Cores 4 Threads]
    Count the number of prime numbers from 
    2 to N.
    Input: N = 100000000
    No of process : Time Taken
                1 : 1.913 seconds 
                2 : 1.547 seconds
                3 : 1.478 seconds
                4 : 1.402 seconds
                5 : 1.433 seconds

    Input: N = 1000000000
    No of process : Time Taken
                1 : 21.411 seconds 
                2 : 16.635 seconds
                3 : ~~~

    To compile: mpicc -o bin/soe src/soe.c -Iinclude -lm
    To run:     mpirun -np [num of process] bin/soe [user input]

