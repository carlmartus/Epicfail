#include "libmain.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>

#define FLAG_GENERATED 1
#define FLAG_VERSION 2

static unsigned flags;
static ef_cb_t callback;
static int report_count[RES_COUNT];

void ef_start()
{
	struct sigaction act;
	flags = 0;
	callback = NULL;

    act.sa_handler = sighandle;
    sigaction(SIGINT, &act, 0);
    sigaction(SIGFPE, &act, 0);
    sigaction(SIGSEGV, &act, 0);
    sigaction(SIGHUP, &act, 0);
    sigaction(SIGABRT, &act, 0);
}

void ef_stop()
{
	summary_t res;
	memcpy(res.cases, report_count, sizeof(res.cases));
	ef_trigger(EVENT_INFO, EF_STATUS, &res);
}

void ef_input(const char *msg)
{
	char ch, full_name[512];

	if ((flags & FLAG_GENERATED) == 0 && strcmp(msg, "-g") == 0) {
		generate_make();
		flags |= FLAG_GENERATED;
		return;
	}

	if ((flags & FLAG_VERSION) == 0 && strcmp(msg, "-v") == 0) {
		print_version();
		flags |= FLAG_VERSION;
		return;
	}

	ch = msg[0];
	if (ch == '.' || ch == '/') {
		runlibrary(msg);
	} else {
		sprintf(full_name, "./%s", msg);
		runlibrary(full_name);
	}
}

void ef_set_callback(ef_cb_t cb)
{
	callback = cb;
}

void ef_trigger(int report, int code, void *ptr)
{
	if (report == EVENT_REPORT) {
		report_count[code]++;
	}

	if (callback) {
		callback(report, code, ptr);
	}
}

