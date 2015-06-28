
BASEDIR:=../

TEST_OBJECTS:=test.o attohttp.o test_attohttp.o

HEADER_FILES:=test.h
TEST_TARGET:=attohttp
TEST_NAME:=




CFLAGS_TARGET+=-fprofile-arcs -ftest-coverage -Wall -Werror -pedantic -std=gnu11
CFLAGS+=-D_TEST_ \
	-I$(shell pwd) \
	-I$(BASEDIR)/src \
	-Wno-pointer-to-int-cast
CFLAGS_TEST+= -Wall -Werror
LDFLAGS+=
GCC:=gcc $(CFLAGS)


all: clean doc test junit

test:
	$(MAKE) -C test/ test

junit:
	$(MAKE) -C test/ junit

doc:
	doxygen Doxyfile

clean:
	$(MAKE) -C test/ clean
	rm -Rf build doc

.PHONY: clean test junit docs