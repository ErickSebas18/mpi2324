#include <mpi.h>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>

std::vector<int> read_file() {
    std::fstream fs("datos.txt", std::ios::in);
    std::string line;
    std::vector<int> ret;
    while (std::getline(fs, line)) {
        ret.push_back(std::stoi(line));
    }
    fs.close();
    return ret;
}

std::vector<int> calcular_histograma(int* datos, int size){
    std::vector<int> histo(size);
    for(int i=0;i<size;i++){
        histo[datos[i]]++;
    }
    return histo;
}

int main( int argc, char *argv[])
{
    int rank, nprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    
    printf("Rank: %d/%d\n", rank, nprocs);

    std::vector<int> datos;
    std::vector<int> datos_local;
    int real_size;
    int block_size;
    int padding = 0;

    if(rank == 0){
        datos = read_file();

        if(datos.size() % nprocs != 0){
            real_size = std::ceil((double)datos.size()/nprocs)*nprocs;
            block_size = real_size / nprocs;
            padding = real_size - datos.size();
        }

        std::printf("RANK_%d: max_items: %ld, real_size: %d, block_size: %d, padding: %d\n",
                    rank, datos.size(), real_size, block_size, padding);
    
        MPI_Send(&padding, 1 , MPI_INT , nprocs-1 , 0 , MPI_COMM_WORLD);
    }
    MPI_Bcast(&block_size, 1 , MPI_INT , 0 , MPI_COMM_WORLD);
    datos_local.resize(block_size);

    MPI_Scatter(datos.data(), block_size, MPI_INT, datos_local.data(), block_size, MPI_INT, 0, MPI_COMM_WORLD);

    int new_size = block_size;

    if(rank == nprocs-1){
        MPI_Recv( &padding, 1 , MPI_INT , 0 , MPI_ANY_TAG , MPI_COMM_WORLD , MPI_STATUS_IGNORE);
        new_size = block_size - padding;
    }

    std::printf("RANK_%d calculando histograma, block_size=%d \n", rank, new_size);
    auto histograma_local = calcular_histograma(datos_local.data(), new_size);
   
    std::vector<int> histograma;

    if(rank == 0){
        histograma.resize(101);
    }
    
    MPI_Reduce(histograma_local.data(), histograma.data() , 101, MPI_INT , MPI_SUM , 0, MPI_COMM_WORLD);
    
    double porcentaje = 0.0;

    if(rank == 0){
        std::cout << "Valor\tFrecuencia\tPorcentaje\n";
        for(int index = 0; index<101;index++){
            porcentaje = ((double)(histograma[index]) / datos.size())*100;
            std::cout << index << "\t" << histograma[index] << "\t\t" << porcentaje << "\n";
        }
    }
    MPI_Finalize();
    
    return 0;
}