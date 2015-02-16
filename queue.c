#include <stdlib.h>

#include "queue.h"

event_list* create_event_list()
{
	event_list *ev_list = (event_list *)malloc(sizeof(event_list));
	if (ev_list == NULL)
		return NULL;
	
	ev_list->start = NULL;
	return ev_list;
}
