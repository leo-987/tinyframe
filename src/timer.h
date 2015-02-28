#ifndef __TIMER_H__
#define __TIMER_H__

enum timer_options {
	TIMER_OPT_NONE,		/* 超时不处理 */
	TIMER_OPT_ONCE,		/* 超时处理一次 */
	TIMER_OPT_REPEAT	/* 超时重复处理 */
};

typedef struct timer_t timer;

typedef struct timer_t {
	struct timeval timeout_abs;		/* 超时绝对时间 */
	struct timeval timeout_rel;		/* 超时相对时间 */
	enum timer_options option;		/* 是否重复触发 */

	/* 超时回调函数 */
	void (*timeout_handler)(void *arg);
	void *arg;

	timer *prev;
	timer *next;
	
}timer;

timer *timer_new(struct timeval timeout, void (*timeout_handler)(void *arg),
					void *arg, enum timer_options option);
void timer_reset(timer *t);

long tv_to_msec(const struct timeval *tv);

#endif

