
all: clean doc test junit

test:
	$(MAKE) -C test/noauth test
	$(MAKE) -C test/basicauth test

junit:
	$(MAKE) -C test/auth junit
	$(MAKE) -C test/basic junit

doc:
	doxygen Doxyfile

devdoc:
	doxygen Doxyfile.dev

clean:
	$(MAKE) -C test/noauth clean
	$(MAKE) -C test/basicauth clean
	rm -Rf build doc

.PHONY: clean test junit docs