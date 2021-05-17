#ifndef _TRAVEL_MONITOR_LIBRARY
#define _TRAVEL_MONITOR_LIBRARY




typedef struct Monitor{
	int numCountries;
    char** countries;

}Monitor;

void MONITOR_init(Monitor* monitor,int numCountries);

void MONITOR_print_all(Monitor *monitor, unsigned int numMonitors);

void MONITOR_destroy(Monitor* monitor);


#endif // _TRAVEL_MONITOR_LIBRARY