#include <iostream>
#include <mpi.h>
#include <vector>

int sumar(const std::vector<int>& v1, int n) {
    int suma = 0;
    for (int i = 0; i < n; i++) {
        suma += v1[i];
    }
    return suma;
}

int calcularStart(int rank_id, int nprocs, int tamanio) {
    int base_size = tamanio / nprocs;
    int extra = tamanio % nprocs;

    if (rank_id < extra) {
        return rank_id * (base_size + 1);
    } else {
        return rank_id * base_size + extra;
    }
}

int calcularSize(int rank_id, int nprocs, int tamanio) {
    int base_size = tamanio / nprocs;
    int extra = tamanio % nprocs;

    if (rank_id < extra) {
        return base_size + 1;
    } else {
        return base_size;
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, nprocs;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    std::vector<int> datos;
    const int tamanio = 100;

    if (rank == 0) {
        datos.resize(tamanio);
        for (int i = 0; i < tamanio; i++) {
            datos[i] = i;
        }

        for (int rank_id = 1; rank_id < nprocs; rank_id++) {
            int start = calcularStart(rank_id, nprocs, datos.size());
            int size = calcularSize(rank_id, nprocs, datos.size());
            MPI_Send(&datos[start], size, MPI_INT, rank_id, 0, MPI_COMM_WORLD);
        }

        int suma_total = sumar(datos, calcularSize(0, nprocs, datos.size()));

        for (int rank_id = 1; rank_id < nprocs; rank_id++) {
            int suma_parcial;
            MPI_Recv(&suma_parcial, 1, MPI_INT, rank_id, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            suma_total += suma_parcial;
        }

        std::cout << "La suma total es: " << suma_total << std::endl;

    } else {
        int size = calcularSize(rank, nprocs, tamanio);
        std::vector<int> datos_local(size);

        MPI_Recv(datos_local.data(), size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int suma_parcial = sumar(datos_local, size);

        MPI_Send(&suma_parcial, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}
