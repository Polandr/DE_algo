#ifndef PARALLEL_H
#define PARALLEL_H

#ifndef ROOT_PROC
#define ROOT_PROC 0
#endif
#include <cstddef>
#include <iostream>
#include <vector>
#include <mpi.h>

struct Processors
{
	static bool initialized;
	static bool cartesian;

	static int root;

	static int proc_coord;
	static int proc_num;

	static std::vector<int> cart_coord;
	static std::vector<int> cart_num;

public:

	static void start (int rt = ROOT_PROC);
	static void start_cartesian (std::vector<int> _dims, int rt = ROOT_PROC);
	static void finish ();

	static int coord () { return proc_coord; }
	static int cart_coords (int i) { return cart_coord[i]; }
	static std::vector<int> cart_coords () { return cart_coord; }
	static int size () { return proc_num; }

	static void abort () { MPI_Abort(MPI_COMM_WORLD,-1); }
	static void barrier () { MPI_Barrier(MPI_COMM_WORLD); }
	static bool is_root () { return proc_coord == root; }

	// Shift on <_shift> positions in linear topology
	static void shift (int _shift, int* src, int* dst);
	// Shift on <_shift> positions in cartesian topology in <dim>th dimension
	static void cart_shift (int _shift, int dim, int* src, int* dst);

	// Broadcast:
	static void bcast_from_root(void* data, std::size_t data_size);

	// Exchange operations:
	static void rotation_exchange_doubles (int rot_step, double* data, int num);
	static void rotation_exchange (unsigned int rot_step, void* data, std::size_t data_size);
	static void neighbor_exchange (void* data, std::size_t data_size);

	// Reduction operations:
	static double sum_reduction (double val);
	static double maximum (double val);
	static double average (double val);
	static double minimum (double val);
	static void gather_to_root(void* src_data, void* dst_data, std::size_t data_size);

	// Get start of parition of <num> for process with number <proc>
	static int start_partition (int num, int proc);
	// Get parition of <num> for process with number <proc>
	static int partition (int num, int proc);

	static void root_print (const char* string, std::ostream& out = std::cout);
	static void endline (std::ostream& out = std::cout);
	static void print_info (std::ostream& out = std::cout);

};

#endif // PARALLEL_H
