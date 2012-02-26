#ifndef MAKE_H
#define MAKE_H

#define MAKE_FILE "Makefile"
#define MAKE_CONTENT \
"SRC=$(wildcard *.c)\n\n" \
"CC=cc\n" \
"LIBS=$(SRC:%.c=test_%.ef)\n" \
"OBJS=# Add objectfiles to be included in testing.\n\n" \
".PHONY: all clean re\n\n" \
"all: $(LIBS)\n\n" \
"test_%.ef: %.c\n" \
"	@echo \"Building $@ from $<\"\n" \
"	@$(CC) -shared -fPIC -Wl,-soname,$@ -o $@ $< $(OBJS)\n\n" \
"clean:\n" \
"	$(RM) $(LIBS)\n\n" \
"re: clean all\n"

#define C_FILE "skeleton.c"
#define C_CONTENT \
"#include \"epicfail.h\"\n\n"\
"void start()\n"\
"{\n"\
"}\n\n"\
"void stop()\n"\
"{\n"\
"}\n\n"\
"static int test_skeleton()\n"\
"{\n"\
"	return 0; // Success\n"\
"}\n\n"\
"epicfail_case_t manifest[] = {\n"\
"	{ \"Sample test case\", test_skeleton },\n"\
"	{ 0, 0 }, // Final row\n"\
"};\n"

#define H_FILE "epicfail.h"
#define H_CONTENT \
"#define H_CONTENT = \n"\
"#ifndef EPICFAIL_H\n"\
"#define EPICFAIL_H\n"\
"\n"\
"typedef int (*epicfail_func_t) ();\n"\
"typedef struct {\n"\
"	const char *title;\n"\
"	epicfail_func_t call;\n"\
"} epicfail_case_t;\n"\
"\n"\
"#endif\n"

#endif

