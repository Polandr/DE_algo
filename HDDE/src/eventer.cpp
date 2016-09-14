#include <eventer.h>
#include <mpi.h>

using namespace std;

double Eventer::evolution_time = 0;
bool Eventer::evolution_stopped = false;
std::vector<Event_id> Eventer::id_vec;
std::vector<int> Eventer::counters;
std::vector<double> Eventer::times;
std::vector<bool> Eventer::stops;

int Eventer::find (Event_id e)
{
	for (int i = 0; ; ++i)
	{
		if (id_vec[i] == e)
			return i;
		if (i == id_vec.size())
			return -1;
	}
}

void Eventer::start_evolution ()
{
	evolution_time = MPI_Wtime();
	evolution_stopped = false;
}
double Eventer::get_evolution_time ()
{
	if (evolution_stopped)
		return evolution_time;
	else
		return MPI_Wtime() - evolution_time;
}
double Eventer::stop_evolution ()
{
	evolution_stopped = true;
	evolution_time = MPI_Wtime() - evolution_time;
	return evolution_time;
}

Event_id Eventer::start_event ()
{
	Event_id free_id = 0;
	for (int i = 0; i < id_vec.size(); ++i, ++free_id)
		if (id_vec[i] != free_id)
			break;

	double cur_time = MPI_Wtime();
	id_vec.insert(id_vec.begin() + free_id, free_id);
	times.insert(times.begin() + free_id, cur_time);
	counters.insert(counters.begin() + free_id, 0);
	stops.insert(stops.begin() + free_id, false);
	return free_id;
}
double Eventer::stop_event (Event_id e)
{
	int i = find(e);
	stops[i] = true;
	times[i] = MPI_Wtime() - times[i];
	return times[i];
}
void Eventer::erase_event (Event_id e)
{
	int i = find(e);
	id_vec.erase(id_vec.begin() + i);
	times.erase(times.begin() + i);
	counters.erase(counters.begin() + i);
	stops.erase(stops.begin() + i);
}
int Eventer::inc_counter (Event_id e)
{
	int i = find(e);
	counters[i]++;
	return counters[e];
}
int Eventer::get_counter (Event_id e)
{
	int i = find(e);
	return counters[i];
}
double Eventer::get_time (Event_id e)
{
	int i = find(e);
	if (stops[i])
		return times[i];
	else
		return MPI_Wtime() - times[i];
}
