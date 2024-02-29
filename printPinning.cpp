#include <iostream>
#include <iomanip>
#include <omp.h>
#include <unistd.h>
#include <thread>
#include <mpi.h>

int main(int argc, char **argv) {

  MPI_Init(&argc, &argv);

  // CONFIGURE THIS FOR THE CURRENT SYSTEM
  const size_t threadsPerCore = THREADS_PER_CORE;
  const size_t numNUMA = NUMA_NODES;

  // Number of cores, including virtual ones per NUMA node.
  const size_t numCPU = std::thread::hardware_concurrency();
  const size_t numCores = numCPU / threadsPerCore;
  const size_t coresPerNuma = numCores / numNUMA;

  int myRank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  int numRanks = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &numRanks);
  int widthRanks = std::to_string(numRanks).size();
  int numThreads = omp_get_max_threads();
  int widthThreads = std::to_string(numThreads).size();

  // emulate a MPI critical section
  for(int rank = 0; rank < numRanks; ++rank) {
    if (rank == myRank) {
#pragma omp parallel
      {
        const auto threadId = omp_get_thread_num();
        const auto cpuID = sched_getcpu();
        const auto core = cpuID % numCores;
        const auto numaNode = core / coresPerNuma;
#pragma omp critical
        std::cout << "Rank: " << std::setw(widthRanks) << std::right << myRank
          << " Thread " << std::setw(widthThreads) << threadId
          << " CPU " << std::setw(3) << cpuID
          << " Core " << std::setw(2) << core
          << " NUMA node " << numaNode
          << std::endl;

      }
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }
  MPI_Finalize();
}

