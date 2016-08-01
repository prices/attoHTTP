
BASEDIR:=../../

TEST_OBJECTS:=test.o attohttp.o test_attohttp.o test_attohttpserversentevents.o test_attohttpjson.o test_attohttpAPI.o test_attohttpparams.o test_attohttpstress.o

HEADER_FILES:=test.h $(BASEDIR)src/attohttp.h
TEST_TARGET:=attohttp
TEST_NAME:=




CFLAGS_TARGET+=-fprofile-arcs -ftest-coverage -Wall -Werror -pedantic -std=c11
CFLAGS+=-D_TEST_ \
	-I$(shell pwd) \
	-I$(BASEDIR)src \
	-Wno-pointer-to-int-cast
CFLAGS_TEST+= -Wall -Werror -std=gnu11
LDFLAGS+=
GCC:=gcc $(CFLAGS)


all: coverage

coverage: clean junit

test: run_test
	./run_test -l standard

junit: run_test
	rm -f *-Results.xml
	@echo "Test output is in $(TEST_TARGET)$(TEST_NAME)-Results.xml"
	-./run_test -l junit > $(TEST_TARGET)$(TEST_NAME)-Results.xml

run_test: $(TEST_OBJECTS) $(HEADER_FILES)
	$(GCC) $(LDFLAGS) $(CFLAGS_TARGET) -o $@ $(TEST_OBJECTS)

$(TEST_TARGET).o: $(BASEDIR)/src/$(TEST_TARGET).c $(BASEDIR)/src/$(TEST_TARGET).h $(HEADER_FILES)
	$(GCC) $(CFLAGS_TARGET) -c $< -o $@


%.o : %.c %.h
	$(GCC) $(CFLAGS_TEST) -c $< -o $@

%.o : %.c
	$(GCC) $(CFLAGS_TEST) -c $< -o $@

clean:
	rm -f *~ *.o run_test *.gcda *.gcno *Results.xml *.orig

.PHONY: clean junit test coverage