#ifndef EVENTER_H
#define EVENTER_H

#include <vector>

typedef int Event_id; 

class Eventer
{
	static double evolution_time;
	static bool evolution_stopped;

	static std::vector<Event_id> id_vec;
	static std::vector<int> counters;
	static std::vector<double> times;
	static std::vector<bool> stops;

	static int find (Event_id e);

public:

	static void start_evolution ();
	static double get_evolution_time ();
	static double stop_evolution ();

	static Event_id start_event ();
	static double stop_event (Event_id e);
	static void erase_event (Event_id e);

	static int inc_counter (Event_id e);
	static int get_counter (Event_id e);
	static double get_time (Event_id e);
};

#endif // EVENTER_H
