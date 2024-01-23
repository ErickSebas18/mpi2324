#include <iostream>
#include <mpi.h>
#include <vector>
#include <cmath>

#define MATRIX_DIMENSION 25

void matriz_mult(double* A, double* b, double* c, int rows, int cols){
    for(int  i =0; i<rows;i++){
        double tmp = 0;
        for(int j = 0; j<cols;j++){
            tmp = tmp + A[i*cols+j]*b[j];
        }
        c[i] = tmp;
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, nprocs;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int rows_per_rank;
    int rows_alloc = MATRIX_DIMENSION;
    int padding=0;

    if (MATRIX_DIMENSION%nprocs != 0){
        rows_alloc = std::ceil((double)MATRIX_DIMENSION/nprocs)*nprocs;
        padding = rows_alloc - MATRIX_DIMENSION;
    }

    rows_per_rank = rows_alloc / nprocs;

    if(rank == 0){
        //Imprimir informacion
        std::printf("Dimension: %d, rows_alloc: %d, rows_per_rank: %d, padding: %d\n", MATRIX_DIMENSION, rows_alloc, rows_per_rank, padding);

        //Crear la matriz A:25x28
        std::vector<double> A(MATRIX_DIMENSION*rows_alloc);
        //Crear la matriz B:25
        std::vector<double> b(MATRIX_DIMENSION);
        //Crear la matriz resultante C:28
        std::vector<double> c(rows_alloc);

        for(int i=0;i<MATRIX_DIMENSION;i++) {
            for(int j = 0; j <MATRIX_DIMENSION; j++){
                int index = i*MATRIX_DIMENSION+j;
                A[index] = i;
            }
        }
        for(int i = 0; i<MATRIX_DIMENSION;i++) b[i]=1;

        //enviar la matriz A
        MPI_Scatter(A.data(), MATRIX_DIMENSION*rows_per_rank,MPI_DOUBLE,
                    MPI_IN_PLACE, 0, MPI_DOUBLE,
                    0, MPI_COMM_WORLD);

        //enviar la matriz b
        MPI_Bcast(b.data(), MATRIX_DIMENSION, MPI_DOUBLE,0, MPI_COMM_WORLD);

        //Realizar el calculo c=Axb

        matriz_mult(A.data(), b.data(), c.data(),rows_per_rank, MATRIX_DIMENSION);


        //Recibir los resultados parciales
        MPI_Gather(MPI_IN_PLACE, 0, MPI_DOUBLE,
                   c.data(), rows_per_rank, MPI_DOUBLE,
                   0, MPI_COMM_WORLD);

        c.resize(MATRIX_DIMENSION);

        //imprimir el resultado
        std::printf("El resultado es:\n");
        for(int  i = 0; i<MATRIX_DIMENSION; i++){
            std::printf("%.0f, ", c[i]);
        }

    } else {
        std::vector<double> Alocal(MATRIX_DIMENSION*rows_per_rank);
        std::vector<double> blocal(MATRIX_DIMENSION);
        std::vector<double> clocal(rows_per_rank);

        //Recibir la matriz A
        MPI_Scatter(nullptr, 0, MPI_DOUBLE,
                    Alocal.data(), MATRIX_DIMENSION*rows_per_rank, MPI_DOUBLE,
                    0, MPI_COMM_WORLD);

        std::printf("RANK_%d: [%.0f..%.0f]\n", rank, Alocal[0], Alocal.back());

        //Recibir la matriz b
        MPI_Bcast(blocal.data(), MATRIX_DIMENSION, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        //Realizar el calculo c=Axb
        int rows_per_rank_tmp = rows_per_rank;

        if(rank == nprocs-1){
            rows_per_rank_tmp = MATRIX_DIMENSION - (rank*rows_per_rank);
            //rows_per_rank_tmp = rows_per_rank-padding;
        }

        matriz_mult(Alocal.data(), blocal.data(), clocal.data(),rows_per_rank, MATRIX_DIMENSION);

        //Enviar el resultado parcial
        MPI_Gather(clocal.data(), rows_per_rank, MPI_DOUBLE,
                   nullptr, 0, MPI_DOUBLE,
                   0, MPI_COMM_WORLD);
    }
    MPI_Finalize();

    return 0;
}

