
all: clean doc test junit

test:
	$(MAKE) -C test/noauth test
	$(MAKE) -C test/basicauth test
	$(MAKE) -C test/digestauth test

junit:
	$(MAKE) -C test/noauth junit
	$(MAKE) -C test/basicauth junit
	$(MAKE) -C test/digestauth junit

doc:
	doxygen Doxyfile

devdoc:
	doxygen Doxyfile.dev

clean:
	$(MAKE) -C test/noauth clean
	$(MAKE) -C test/basicauth clean
	$(MAKE) -C test/digestauth clean
	rm -Rf build doc

.PHONY: clean test junit docs
