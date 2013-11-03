#include <stdio.h>
#include "epicfail.h"

static void
print_complete(int id, int code, const char *title)
{
	const char *stamp = "???";
	switch (code) {
		case RES_OK :		stamp = "32mPass";		break;
		case RES_ASSERT :	stamp = "35mAsrt";	break;
		case RES_RETURN :	stamp = "35mRetr";	break;
		case RES_FATAL :	stamp = "31mFata";	break;
		case RES_START :	stamp = "31mStar";	break;
		case RES_STOP :		stamp = "31mStop";	break;
		default :			stamp = "????";
	}

	printf(" %3d [\x1B[1;%s\x1B[0m]\t%s\n", id, stamp, title);
}

static void
info_summary(summary_t *sum)
{
	int i, pass, total = 0;
	double proc;
	pass = sum->cases[RES_OK];
	for (i=0; i<RES_COUNT; i++) {
		total += sum->cases[i];
	}

	proc = 100.0;
	if (total > 0) {
		proc = 100.0 * ((double) pass) / (double) total;
	}
	printf("\n   Testcases: %d/%d (%.0f%%)\n", pass, total, proc);
}

static void
info_msg(int code, void *ptr)
{
	switch (code) {
		case EF_TRIVIAL : break;
		case EF_STATUS :
				info_summary((summary_t*) ptr);
				break;

		case EF_STEP :
		case EF_GENERATE :
				printf("%s\n", (const char*) ptr);
				break;

		default:
				printf("%4d :: %s\n", code, (const char*) ptr);
				break;
	}
}

static void
callback(int report, int code, void *ptr)
{
	result_t *res;

	switch (report) {
		case EVENT_REPORT :
			res = ptr;
			print_complete(res->id, res->code, res->title);
			break;

		case EVENT_INFO :
			info_msg(code, ptr);
			break;
	}
}

static void
run(const char *file_name)
{
	ef_input(file_name);
}

int
main(int argc, char **argv)
{
	int i;
	if (argc <= 1) {
		printf("Usage: %s testfile1 testfile2 ..\n", argv[0]);
		return 1;
	}

	ef_start();
	ef_set_callback(callback);

	for (i=1; i<argc; i++) {
		run(argv[i]);
	}

	ef_stop();
	return 0;
}

