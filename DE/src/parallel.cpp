#include <iostream>
#include <cassert>
#include <cstdlib>
#include <parallel.h>

#ifndef ROOT_PROC
#define ROOT_PROC 0
#endif
#define ROT_EXCH_TAG 1024
#define NBR_EXCH_TAG 1025

using namespace std;

MPI_Comm cart_communicator;

bool Processors::initialized = false;
bool Processors::cartesian = false;

int Processors::root = ROOT_PROC;

int Processors::proc_coord = 0;
int Processors::proc_num = 1;

vector<int> Processors::cart_coord;
vector<int> Processors::cart_num;

int mod (int a, int b)
{
	if (a >= 0)
		return a % b;
	else
		return (b - (-a % b)) % b;
}

void init_MPI()
{
	int inited;
	MPI_Initialized(&inited);
	if (!inited)
	{
		int argc = 0;
		char** argv = NULL;
		MPI_Init(&argc,&argv);
	}	
}

void Processors::start (int rt)
{
	if (!initialized)
	{
		root = rt;
		init_MPI();

		MPI_Comm_size(MPI_COMM_WORLD,&proc_num);
		MPI_Comm_rank(MPI_COMM_WORLD,&proc_coord);

		vector<int> v(1);
		v[0] = proc_num;
		cart_num = v;
		v[0] = proc_coord;
		cart_coord = v;

		if (is_root())
			cout << "Communicator initialized!\n" << flush;
		initialized = true;
	}
}
void Processors::start_cartesian (vector<int> _dims, int rt)
{
	if (!initialized)
	{
		root = rt;
		init_MPI();
		MPI_Comm_size(MPI_COMM_WORLD,&proc_num);
		int grid_size = 1;
		for (int i = 0; i < _dims.size(); ++i)
			grid_size *= _dims[i];
		assert(grid_size == proc_num);

		// Creation of grid communicator
		cart_num = _dims;
		int *dims, *periods;
		dims = (int*) malloc(cart_num.size()*sizeof(int));
		periods = (int*) malloc(cart_num.size()*sizeof(int));
		for (int i = 0; i < cart_num.size(); ++i)
		{
			dims[i] = cart_num[i];
			periods[i] = 1;
		}
		MPI_Cart_create(MPI_COMM_WORLD, cart_num.size(), dims, periods, 0, &cart_communicator);
		free(dims);
		free(periods);

		// Coordinates
		int *coords;
		coords = (int*) malloc(cart_num.size()*sizeof(int));
		MPI_Cart_coords(cart_communicator, proc_coord, cart_num.size(), coords);
		cart_coord.resize(cart_num.size());
		for (int i = 0; i < cart_num.size(); ++i)
			cart_coord[i] = coords[i];

		if (is_root())
			cout << "Cartesian communicator initialized!\n" << flush;

		cartesian = true;
		initialized = true;
	}
}
void Processors::finish ()
{
	barrier();
	if (cartesian)
		MPI_Comm_free(&cart_communicator);
	MPI_Finalize();
	if (is_root())
		cout << "Communicator finalized!\n" << flush;
}

// Shift on <_shift> positions in linear topology
void Processors::shift (int _shift, int* src, int* dst)
{
	*src = mod(proc_coord - _shift, proc_num);
	*dst = mod(proc_coord + _shift, proc_num);
}
// Shift on <_shift> positions in cartesian topology in <dim>th dimension
void Processors::cart_shift (int _shift, int dim, int* src, int* dst)
{
	MPI_Cart_shift(cart_communicator, dim, _shift, src, dst);
}

void Processors::bcast_from_root(void* data, std::size_t data_size)
{
	MPI_Bcast(data, data_size, MPI_BYTE, Processors::root, MPI_COMM_WORLD);
}

void Processors::rotation_exchange (unsigned int step, void* data, std::size_t data_size)
// Rotation of exchange data using ring topology
{
	if (step != 0)
	{
		int src, dst;
		shift(step, &src, &dst);
		int send_tag, recv_tag;
		send_tag = recv_tag = ROT_EXCH_TAG;
		MPI_Sendrecv_replace(data, data_size, MPI_BYTE, dst, send_tag, src, recv_tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
}
void Processors::neighbor_exchange (void* data, std::size_t data_size)
// Exchange in pairs distanced for step processes between each other
{
	int src, dst;
	shift(1, &src, &dst);
	if (proc_num % 2 == 0)
	// Left member of exchange
		src = dst;
	else
	// Right member of exchange
		dst = src;
	int send_tag, recv_tag;
	send_tag = recv_tag = NBR_EXCH_TAG;
	MPI_Sendrecv_replace(data, data_size, MPI_BYTE, dst, send_tag, src, recv_tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

double Processors::maximum (double val)
{
	double out;
	MPI_Allreduce(&val, &out, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
	return out;
}
double Processors::average (double val)
{
	double out;
	MPI_Allreduce(&val, &out, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	out /= size();
	return out;
}
double Processors::minimum (double val)
{
	double out;
	MPI_Allreduce(&val, &out, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
	return out;
}

void Processors::gather_to_root(void* src_data, void* dst_data, std::size_t data_size)
{
	MPI_Gather(src_data, data_size, MPI_BYTE, dst_data, data_size, MPI_BYTE, Processors::root, MPI_COMM_WORLD);
}

void Processors::root_print (const char* string, ostream& out)
{
	if (is_root())
		out << string << flush;
}

void Processors::endline (ostream& out)
{
	if (is_root())
		out << endl << flush;
}

void Processors::print_info (ostream& out)
{
	if (is_root())
	{
		out << proc_num << " MPI processes\n";
		if (cartesian)
			out << "With cartesian topology\n";
		out << "Process # " << root << " is a root process\n";
		out << flush;
	}
}
