#ifndef __TIMER_H__
#define __TIMER_H__

#include "servermanager.h"

enum timer_options {
	TIMER_OPT_NONE,		/* 超时不处理 */
	TIMER_OPT_ONCE,		/* 超时处理一次 */
	TIMER_OPT_REPEAT	/* 超时重复处理 */
};

typedef struct server_manager_t server_manager;
typedef struct timer_t timer;
typedef void (*timeout_callback_pt)(timer *t, void *arg);

struct timer_t {
	struct timeval timeout_abs;		/* 超时绝对时间 */
	struct timeval timeout_rel;		/* 超时相对时间 */
	enum timer_options option;		/* 是否重复触发 */

	/* 超时回调函数 */
	timeout_callback_pt timeout_handler;
	void *arg;

	timer *prev;
	timer *next;
};

timer *timer_new(struct timeval timeout, timeout_callback_pt timeout_handler,
					void *arg, enum timer_options option);
void timer_reset(timer *t);
long tv_to_msec(const struct timeval *tv);
void timer_add(server_manager *manager, timer *t);
void timer_remove(timer *t);

#endif

