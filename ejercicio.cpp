#include<iostream>
#include <mpi.h>
#include <vector>
#include <cmath>
#define MAX_ITEMS 25

int sumar(int* tmp, int n){

    int suma = 0;
    for(int i = 0; i<n; i++){
        suma += tmp[i];
    }
    return suma;
}
int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, nprocs;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);


    int real_size=(int)(MAX_ITEMS/nprocs)*nprocs;;
    int block_size=(int)real_size/nprocs;
    int padding=0;

    if(MAX_ITEMS%nprocs!=0){
        real_size = std::ceil((double)MAX_ITEMS/nprocs)*nprocs;
        block_size=real_size/nprocs;
        padding =real_size - MAX_ITEMS;
    }

    std::vector<int> data;
    std::vector<int> data_local(block_size);

    if(rank==0){
        data.resize(real_size);
        std::printf("DimensiÃ³n: %d, real_size: %d, block_size: %d, padding: %d\n",
                    MAX_ITEMS, real_size, block_size, padding);
        for(int i = 0; i<MAX_ITEMS;i++){
            data[i] = i;
        }
    }
    //Enviar datps
    MPI_Scatter(data.data(), block_size, MPI_INT, data_local.data(), block_size, MPI_INT, 0, MPI_COMM_WORLD);
    //Calcular la suma parcial
    //El ultimo rank no debe sumar tod o el bloque, ya que tiene padding
    if(rank==nprocs-1){
        block_size = block_size-padding;
    }

    int suma_parcial = sumar(data_local.data(), block_size);
    std::printf("RANK_%d: suma parcial = %d\n", rank, suma_parcial);
    //Enviar las suma parcial al RANK 0
    //De donde obtenemos los datos,
    //Donde guardamos la suma

    int suma_total = 0;
    //Solo el rank 0 recibe los datos, el resto no
    MPI_Reduce(&suma_parcial, &suma_total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    //MPI_Allreduce(&suma_parcial, &suma_total, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    if(rank==0){
        std::printf("Suma Total: %d\n", suma_total);
    }
    MPI_Finalize();

    return 0;
}