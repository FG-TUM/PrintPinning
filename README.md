# PrintPinning

Mini tool to quickly analyze where threads are placed on the hardware. 

## Requirements

- C++ compiler
- OpenMP
- MPI
- (optional) lscpu
- (optional) grep

## Build

```bash
mkdir build && cd build
cmake ..
make
```

## Run

```bash
mpirun -n 2 ./printPinning
```

This prints a list of all threads that were launched and their hardware location.

### Example usage

```bash
OMP_PROC_BIND=true          \
OMP_PLACES=cores            \
OMP_NUM_THREADS=8           \
mpirun -n 4 ./printPinning  \
    | sort
```
