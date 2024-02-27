#include <mpi.h>
#include <fstream>
#include <vector>
#include <cmath>

std::vector<int> read_file() {
    std::fstream fs("/workspace/mpi2324/datos.txt", std::ios::in);
    std::string line;
    std::vector<int> ret;
    while (std::getline(fs, line)) {
        ret.push_back(std::stoi(line));
    }
    fs.close();
    return ret;
}

int main( int argc, char *argv[])
{
    int rank, nprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    
    printf("Rank: %d/%d\n", rank, nprocs);

    if(rank == 0){
        std::vector<int> datos = read_file();
        int blocksize = std::ceil((double) datos.size() / nprocs);
        for(int i = 1; i < nprocs; i++){
            MPI_Send(&datos[blocksize*i], datos.size(), MPI_INT, i,0, MPI_COMM_WORLD);
            std::cout << "Datos Enviados al Rank: " << i << "\n";
        }
    printf("El blocksize es: %d y el tamanio del archivo es: %ld \n", blocksize, datos.size());
    }

    else {


    }
    
    MPI_Finalize();
    
    return 0;
}