EXE=epicfail

CC=cc
LD=cc
CFLAGS=-Wall -O2 -g
LDFLAGS=-ldl

ODIR=obj
OTOUCH=obj/.d

SRC=$(wildcard *.c)
OBJ=$(SRC:%.c=$(ODIR)/%.o)
DEP=$(OBJ:%.o=%.d)

INSTALL_BIN=/usr/bin/$(EXE)

ZIP_FILE=epicfail.zip
ZIP_CONTENT=$(wildcard *c) $(wildcard *.h) Makefile LICENSE README

.PHONY: all doc zip install uninstall clean re

all: $(EXE)

$(EXE): $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)

-include $(DEP)

$(ODIR)/%.o: %.c $(OTOUCH)
	$(CC) -o $@ -c $< -MMD -MF $(@:%.o=%.d) $(CFLAGS)

$(OTOUCH):
	mkdir -p $(ODIR)
	touch $@

zip: $(ZIP_FILE)

$(ZIP_FILE): clean
	rm -f $(ZIP_FILE)
	zip $@ -r $(ZIP_CONTENT)

install: $(LIBFILE)
	cp -v $(EXE) $(INSTALL_BIN)

uninstall:
	rm -vr $(INSTALL_BIN)

clean:
	$(RM) -r $(ODIR) $(EXE)

re: clean all

