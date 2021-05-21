#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libTM.h"

// Elegxos gia to an to date_tested einai anamesa sta date1 kai date2
// An einai epistrefei 1, alliws epistrefei 0
int is_between_date(char* date_tested, char* date1, char* date2)
{
    int dt, mt, yt, d1, m1, y1, d2, m2, y2;
    int flag=0;
    sscanf(date_tested,"%d-%d-%d",&dt,&mt,&yt);
    sscanf(date1,"%d-%d-%d",&d1,&m1,&y1);
    sscanf(date2,"%d-%d-%d",&d2,&m2,&y2);

    // Efoson to date_tested einai mikrotero toy date2
    if(yt < y2 || (yt == y2 && mt < m2) || (yt == y2 && mt == m2 && dt < d2))
    {
        // An einai kai mikrotero toy date1
        if(y1 < yt)
            return 1;
        if(y1 == yt && m1 < mt)
            return 1;
        if(y1 == yt && m1 == mt && d1 < dt)
            return 1;
    }
    return 0;
}



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