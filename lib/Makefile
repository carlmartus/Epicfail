include ../target.mk

LIBFILE=libepicfail.a
#LIBFILE=libepicfail.so
#CFLAGS+= -fPIC
RMS=$(LIBFILE)
_OBJ=libmain testenv

include ../common.mk

all: $(LIBFILE)

$(LIBFILE): $(OBJ)
	ar -cvq $@ $^

#$(LIBFILE): $(OBJ)
#	$(CC) -shared -Wl,-soname,$@ -o $@ $^

install: $(LIBFILE)
	cp epicfail.h $(INCDIR)
	cp $(LIBFILE) $(LIBDIR)

