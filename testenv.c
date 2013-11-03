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

enum test_type {
	TEST_TYPE_ALL,
	TEST_TYPE_LISTED,
};

static jmp_buf stack;
static int last_signal, teststep;

static test_ception_t suite_start, suite_stop;
static manifest_t *suite_lines;

static void
test_case(testcase_t tc, int id)
{
	int ret;
	char msg[100];
	result_t res;

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

static void
run_some_tests(const char *test_cmd)
{
}

static void
write_file(const char *desc, const char *file, const char *content)
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

void
generate_make()
{
	write_file("Makefile", MAKE_FILE, MAKE_CONTENT);
	write_file("Sample source", C_FILE, C_CONTENT);
	write_file("Header", H_FILE, H_CONTENT);
}

void
print_version()
{
	printf("Epicfail Version %s\n", VERSION);
}

static enum test_type
classify_test(const char *test_string)
{
	return TEST_TYPE_ALL;
	int semis = 0;
	const char *itr = test_string;

	while (1) {
		switch (*itr) {
			case '\0' : break;
			case ':' : semis++; break;
		}
	}

	if (semis > 0) return TEST_TYPE_LISTED;
	return TEST_TYPE_ALL;
}

static void*
open_test_lib(const char *file_name)
{
	// Open library
	char msg[512];
	void *lib = dlopen(file_name, RTLD_LAZY);
	if (lib == NULL) {
		sprintf(msg, "Cannot open library %s", file_name);
		ef_trigger(EVENT_INFO, EF_ERROR, msg);
		fprintf(stderr, "%s\n", dlerror());
		return NULL;
	}

	suite_start = dlsym(lib, "start");
	suite_stop = dlsym(lib, "stop");
	suite_lines = dlsym(lib, "manifest");

	if (suite_lines == NULL) {
		ef_trigger(EVENT_INFO, EF_ERROR, "Cannot get symbols from library");
		return NULL;
	}

	sprintf(msg, "Test %s", file_name);
	ef_trigger(EVENT_INFO, EF_STEP, msg);

	return lib;
}

static int
close_test_lib(void *lib)
{
	return dlclose(lib);
}

static void
run_all_tests(const char *file_name)
{
	void *lib = open_test_lib(file_name);
	if (lib == NULL) return;

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

	close_test_lib(lib);
}

void
runlibrary(const char *test_case)
{
	char msg[512];

	sprintf(msg, "Running %s", test_case);
	ef_trigger(EVENT_INFO, EF_TRIVIAL, msg);

	switch (classify_test(test_case)) {
		case TEST_TYPE_ALL :
			run_all_tests(test_case);
			break;

		case TEST_TYPE_LISTED :
			run_some_tests(test_case);
			break;
	}
}

void
sighandle(int sig)
{
	struct sigaction act;

	// Reset is necessary on assert signals
	last_signal = sig;

	act.sa_flags = SA_SIGINFO;
	sigemptyset(&act.sa_mask);
    act.sa_handler = sighandle;

    sigaction(sig, &act, 0);

	longjmp(stack, 1);
}

