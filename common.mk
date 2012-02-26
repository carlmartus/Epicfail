.PHONY: all clean re install
.PRECIOUS: %/.d

OUT+=$(EXE_FIX)
OBJ=$(_OBJ:%=$(ODIR)/%.o)

BINDIR=/usr/bin/
INCDIR=/usr/include/
LIBDIR=/usr/lib/
CFLAGS=-Wall -O2

all: $(OUT)

$(OUT): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(ODIR)/%.o: %.c $(ODIR)/.d
	$(CC) -o $@ -c $< $(CFLAGS)

%/.d:
	mkdir -p $(@D)
	touch $@

clean:
	$(RM) $(OUT) $(OBJ) $(RMS)

re: clean all

clean-all: clean
	$(RM) -r obj-*

