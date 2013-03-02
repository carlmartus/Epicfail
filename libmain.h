#ifndef LIBMAIN_H
#define LIBMAIN_H

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

typedef void (*test_ception_t)();
typedef int (*test_func_t)();

typedef struct {
	const char *title;
	test_func_t call;
} manifest_t;

typedef struct {
	const char *title;
	test_func_t call;
} testcase_t;

typedef struct {
	int code, id;
	const char *title;
} result_t;

typedef struct {
	int cases[RES_COUNT];
} summary_t;

typedef void (*ef_cb_t) (int report, int code, const char *msg);

void ef_start();
void ef_stop();
void ef_input(const char *msg);
void ef_set_callback(ef_cb_t cb);
void ef_trigger(int report, int code, void *ptr);

void generate_make();
void print_version();
void summary();
void runlibrary(const char *file_name);
void sighandle(int sig);

#endif

