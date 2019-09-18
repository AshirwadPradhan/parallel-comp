#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "soe.h"

int main(int argc, char *argv[])
{

    int id; //process id number
    int process_pool; //number of process
    int num_prime; //range of prime numbers
    int global_prime_count; //global prime count
    double elapsed_time; // record time for parallel execution
    double start_time; //start time
    double end_time; //end time

    MPI_Init(&argc, &argv);

    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &process_pool);

    //check if the range of prime numbers are not satisfied
    if(argc != 2)
    {
        if(!id)
            printf("Err: Range not specified");
        MPI_Finalize();
        exit(1);
    }
    num_prime = atoi(argv[1]);

    //keep track of low, high and size of each block decomposition
    int low_value = BLOCK_LOW(id, process_pool, num_prime - 1) + 2;
    int high_value = BLOCK_HIGH(id, process_pool, num_prime - 1) + 2;
    int size = BLOCK_SIZE(id , process_pool, num_prime - 1);
    printf("\nSize of block for proc[%d]: %d\n", id, size);

    // Check if the process pool is too large for the range
    int proc0_arr = (num_prime - 1)/ process_pool;
    if((proc0_arr + 2) < (int) sqrt((double) num_prime))
    {
        if (!id)
            printf("Err: Too less range with too many process");
        MPI_Finalize();
        exit(1);
    }

    //Allocate memory for the marker array 
    char *marker_space = (char *) malloc(size);
    if(marker_space == NULL)
    {
        printf("Err: Out of Memory");
        MPI_Finalize();
        exit(0);
    }

    //init the marker array
    int i;
    for(i=0; i<size; i++)
        marker_space[i] = 0;
    
    int index; //index of the current prime number
    int current_prime; //current prime number
    if(!id)
        index = 0;
    current_prime = 2;

    int first_multiple;
    do{
        //find the first multiple
        if(current_prime*current_prime > low_value)
            first_multiple = current_prime*current_prime - low_value;
        else{
            if(!(low_value % current_prime))
                first_multiple = 0;
            else
                first_multiple = current_prime - (low_value%current_prime);          
        }
        
        //begin marking the primes
        for(i=first_multiple; i< size; i = i+current_prime)
            marker_space[i] = 1;
        if(!id){
            //find the next unmarked index in the marked array 
            //and set it as the new current prime
            while(marker_space[++index]);
            current_prime = index + 2;
        }

        MPI_Bcast(&current_prime, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }
    while(current_prime*current_prime <= num_prime);

    //count the number of local primes in each block
    // and reduce all to get the sum of number of primes from each block
    int local_prime_count = 0; //local prime count
    for(i=0; i < size; i++)
        if(!marker_space[i])
            local_prime_count++;
    
    MPI_Reduce(&local_prime_count, &global_prime_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    end_time = MPI_Wtime();
    elapsed_time = end_time - start_time;

    if(!id){
        printf("Number of primes in the range [%d] : %d\n", num_prime, global_prime_count);
        printf("Total Time for execution: %10.6f\n", elapsed_time);
    }

    MPI_Finalize();
    return 0;

}