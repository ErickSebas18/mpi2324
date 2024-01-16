#include <iostream>
#include <mpi.h>

int main(int argc, char** argv){
    MPI_Init(&argc, &argv);

    int rank, nprocs;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    //std::printf("Rank %d of %d procs\n", rank, nprocs);

    int data[100];

    if(rank ==0){
       for(int i=0;i<100;i++){
           data[i] = i;
       }

       for(int i = 1; i<nprocs;i++) {
           std::printf("Rank 0 Enviando datos a Rank_%d\n",i);
           MPI_Send(data, 100, MPI_INT, i, 0, MPI_COMM_WORLD);
       }

    } else{
        std::printf("Rank_%d Recibiendo datos\n", rank);
        MPI_Recv(data, 100, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::string str = "";
        for (int i = 0; i < 10; i++){
            //std::printf("%d, ", data[i]);
            str = str + std::to_string(data[i]);
        }
        std::printf("Datos recibidos ==> %s\n", str.c_str());
    }
    MPI_Finalize();

    return 0;
}