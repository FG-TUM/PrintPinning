#include <iostream>
#include <iomanip>
#include <omp.h>
#include <thread>
#include <mpi.h>
#include <map>
#include <vector>

// Macro to string hack
#define STR_(x) #x
#define STR(x) STR_(x)

/**
 * Parsed the intervals of CPU IDs for each NUMA domain.
 *
 * Input format is:
 *  - One line
 *  - Numa domains are separated by |
 *  - Intervals are separated by _
 *  - Start and stop of each interval is separated by -
 *
 * @param numaDomainsStr
 * @return numaDomains<intervals<start,stop>>
 */
std::vector<std::vector<std::pair<int, int>>> parseNumaDomains(const std::string &numaDomainsStr) {
  std::vector<std::vector<std::pair<int, int>>> numaDomains;

  std::istringstream outerStream(numaDomainsStr);
  std::string outerToken;
  while (std::getline(outerStream, outerToken, '|')) {
    std::vector<std::pair<int, int>> innerVector;

    std::istringstream innerStream(outerToken);
    std::string innerToken;
    while (std::getline(innerStream, innerToken, '_')) {
      std::istringstream pairStream(innerToken);
      std::string pairToken;
      std::getline(pairStream, pairToken, '-');

      int first = std::stoi(pairToken);
      std::getline(pairStream, pairToken, '-');
      int second = std::stoi(pairToken);

      innerVector.emplace_back(first, second);
    }

    numaDomains.push_back(innerVector);
  }

  return numaDomains;
}


int main(int argc, char **argv) {

  MPI_Init(&argc, &argv);

  // CONFIGURE THIS FOR THE CURRENT SYSTEM
  const size_t threadsPerCore = THREADS_PER_CORE;

  // Number of cores, including virtual ones per NUMA node.
  const size_t numCPU = std::thread::hardware_concurrency();
  const size_t numCores = numCPU / threadsPerCore;
  const std::vector<std::vector<std::pair<int, int>>> numaDomains = parseNumaDomains(STR(NUMA_DOMAINS));

  int myRank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  int numRanks = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &numRanks);
  int widthRanks = std::to_string(numRanks).size();
  int numThreads = omp_get_max_threads();
  int widthThreads = std::to_string(numThreads).size();

  // emulate a MPI critical section
  for (int rank = 0; rank < numRanks; ++rank) {
    if (rank == myRank) {
#pragma omp parallel
      {
        const auto threadId = omp_get_thread_num();
        const auto cpuID = sched_getcpu();
        const auto core = cpuID % numCores;
        const auto numaNode = [&]() {
            for (int numaID = 0; numaID < numaDomains.size(); ++numaID) {
              for (const auto [start, stop]: numaDomains[numaID]) {
                if (cpuID >= start and cpuID <= stop) {
                  return numaID;
                }
              }
            }
            return -1;
        }();
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

