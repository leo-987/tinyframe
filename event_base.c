#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "event_base.h"
#include "debug.h"

void event_base_run(event_base *base)
{
	while (1)
	{
		int nfds = base->epoller->dispatch(base);
		int i;
		for (i = 0; i < nfds; i++)
		{
			event *ev = base->actives->start;
			ev->event_handler(ev);
			base->actives->start = ev->next;
			
			ev->actives = 0;
			ev->next = NULL;
		}
	}
}

event_base *create_event_base()
{
	event_base *base = (event_base *)malloc(sizeof(event_base));
	if (base == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		return NULL;
	}
	
	base->epoller = create_backend();
	if (base->epoller == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		return NULL;
	}
	
	base->epoller->init();
	base->actives = create_event_list();
	if (base->actives == NULL)
	{
		debug_ret("file: %s, line: %d", __FILE__, __LINE__);
		return NULL;
	}
	
	return base;
}