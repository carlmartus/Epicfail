TARGET=target.mk
LIBDIR=lib
CLIDIR=cli

set_target=\
	echo "CC=$2" > $(TARGET);\
	echo "EXE_FIX=$3" >> $(TARGET);\
	echo "ODIR=obj-$@" >> $(TARGET);\
	echo "Target is now $1"

.PHONY: all lin32 win32 clean

all:

lin32:
	@$(call set_target,Linux 32-bit,cc,)

win32:
	@$(call set_target,Windows 32-bit,i586-mingw32msvc-cc,.exe)

clean:
	$(RM) $(TARGET)

