#include "libmain.h"
#include "gen.h"
#include "config.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <signal.h>
#include <setjmp.h>

enum {
	STEP_START=0,
	STEP_MAIN,
	STEP_STOP,
};

static jmp_buf stack;
static int last_signal, teststep;

static test_ception_t suite_start, suite_stop;
static manifest_t *suite_lines;

static void test_case(testcase_t tc, int id)
{
	int ret;
	char msg[100];
	result_t res;
	res.id = id;

	res.id = id;
	res.title = tc.title;

	sprintf(msg, "Running %s", tc.title);
	ef_trigger(EVENT_INFO, EF_TRIVIAL, msg);

	ret = -1;
	if (!setjmp(stack)) {
		teststep = STEP_START;
		if (suite_start) {
			suite_start();
		}

		teststep = STEP_MAIN;
		ret = tc.call();

		teststep = STEP_STOP;
		if (suite_stop) {
			suite_stop();
		}

		if (ret == 0) {
			res.code = RES_OK;
		} else {
			res.code = RES_RETURN;
		}
	} else {

		switch (teststep) {
			case STEP_START :	res.code = RES_START; break;
			case STEP_STOP :	res.code = RES_STOP; break;
			default:
				res.code = last_signal == SIGABRT ? RES_ASSERT : RES_FATAL;
				break;
		}
	}

	ef_trigger(EVENT_REPORT, res.code, &res);
}

static void run_tests()
{
	int id;
	manifest_t *itr;
	testcase_t tc;

	id = 0;
	itr = suite_lines;
	while (itr->call) {
		tc.title = itr->title;
		tc.call = itr->call;
		test_case(tc, id);

		id++;
		itr++;
	}
}


static void write_file(const char *desc, const char *file, const char *content)
{
	int size;
	FILE *fd;
	char msg[80];

	sprintf(msg, "Generating %s %s", desc, file);
	ef_trigger(EVENT_INFO, EF_GENERATE, msg);

	fd = fopen(file, "w");
	size = strlen(content);
	fwrite(content, size, 1, fd);
	fclose(fd);
}

void generate_make()
{
	write_file("Makefile", MAKE_FILE, MAKE_CONTENT);
	write_file("Sample source", C_FILE, C_CONTENT);
	write_file("Header", H_FILE, H_CONTENT);
}

void print_version()
{
	printf("Epicfail Version %s\n", VERSION);
}

void runlibrary(const char *file_name)
{
	void *lib;
	char msg[512];

	sprintf(msg, "Running %s", file_name);
	ef_trigger(EVENT_INFO, EF_TRIVIAL, msg);

	// Open library
	lib = dlopen(file_name, RTLD_LAZY);
	if (lib == NULL) {
		sprintf(msg, "Cannot open library %s", file_name);
		ef_trigger(EVENT_INFO, EF_ERROR, msg);
		fprintf(stderr, "%s\n", dlerror());
		return;
	}

	suite_start = dlsym(lib, "start");
	suite_stop = dlsym(lib, "stop");
	suite_lines = dlsym(lib, "manifest");

	if (suite_lines == NULL) {
		ef_trigger(EVENT_INFO, EF_ERROR, "Cannot get symbols from library");
		return;
	}

	run_tests();

	// Close library
	dlclose(lib);
}

void sighandle(int sig)
{
	struct sigaction act;

	// Reset is necessary on assert signals
	last_signal = sig;
    act.sa_handler = sighandle;
    sigaction(sig, &act, 0);

	longjmp(stack, 1);
}

