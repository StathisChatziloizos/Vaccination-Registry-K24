#ifndef _TRAVEL_MONITOR_LIBRARY
#define _TRAVEL_MONITOR_LIBRARY




typedef struct Monitor{
	int numCountries;
    char** countries;

}Monitor;

typedef struct Request
{
    char virus_name[20];
    char countryTo[25];
    char date[11];
    int outcome;
}Request;

int is_between_date(char* date_tested, char* date1, char* date2);

int comparator(const void *str1, const void *str2);

void MONITOR_init(Monitor* monitor,int numCountries);

void MONITOR_print_all(Monitor* monitor, unsigned int numMonitors);

int MONITOR_search_country(Monitor* monitor, char* country, unsigned int numMonitors);

void MONITOR_destroy(Monitor* monitor);


#endif // _TRAVEL_MONITOR_LIBRARY