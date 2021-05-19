#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libTM.h"

void MONITOR_init(Monitor* monitor,int num_Countries)
{
    monitor->numCountries = num_Countries;

    // Desmeysh mnhmhs toy  array gia apo8hkeysh twn onomatwn twn xwrwn poy analambanei to Monitor
    monitor->countries = (char**)malloc(num_Countries * sizeof(char*));

    // Desmeysh mnhmhs gia ka8e xwra
    for (int i = 0; i < num_Countries; i++)
    {
        monitor->countries[i] = (char*)malloc(20 * sizeof(char));
    }
    
}

void MONITOR_print_all(Monitor* monitor, unsigned int numMonitors)
{
    printf("---------------------------------\n");
    for (int i = 0; i < numMonitors; i++)
	{
		for (int j = 0; j < monitor[i].numCountries; j++)
		{
			printf("monitor[%d].countries[%d] = %s\n",i,j, monitor[i].countries[j]);
		}
		printf("---------------------------------\n");
		
	}
}

int MONITOR_search_country(Monitor* monitor, char* country, unsigned int numMonitors)
{
    for (int i = 0; i < numMonitors; i++)
	{
		for (int j = 0; j < monitor[i].numCountries; j++)
		{
            if(strcmp(monitor[i].countries[j], country) == 0)
                return i;
		}		
	}
    return -1;
}



void MONITOR_destroy(Monitor* monitor)
{
	
	for (int i = 0; i < monitor->numCountries; i++)
    {
        free(monitor->countries[i]);
    }

    free(monitor->countries);
}