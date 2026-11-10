#ifndef PARALLELCOMPUTING_TIMESTEP_H
#define PARALLELCOMPUTING_TIMESTEP_H
#define REAL_CELL 1

double timestep(int ncells, double g, double sigma, int* restrict celltype,
                double* H, double* U, double* V, double* dx, double* dy);
#endif
