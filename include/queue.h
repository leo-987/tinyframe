#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "event.h"

typedef struct event_list_t {
	event *start;
}event_list;

event_list* create_event_list();

#endif

