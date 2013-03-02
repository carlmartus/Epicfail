#ifndef EPICFAIL_H
#define EPICFAIL_H

enum {
	EVENT_INFO=0,
	EVENT_REPORT,
};

enum {
	EF_TRIVIAL=0,
	EF_STEP,
	EF_ERROR,
	EF_GENERATE,
	EF_STATUS,
};

enum {
	RES_OK=0,
	RES_ASSERT,
	RES_RETURN,
	RES_FATAL,
	RES_START,
	RES_STOP,
	RES_COUNT,
};

typedef struct {
	int code, id;
	const char *title;
} result_t;

typedef struct {
	int cases[RES_COUNT];
} summary_t;

typedef void (*ef_cb_t) (int report, int code, void *ptr);

void ef_start();
void ef_stop();
void ef_input(const char *msg);
void ef_set_callback(ef_cb_t cb);

#endif

