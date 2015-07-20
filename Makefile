
all: clean doc test junit

test:
	$(MAKE) -C test/ test

junit:
	$(MAKE) -C test/ junit

doc:
	doxygen Doxyfile

devdoc:
	doxygen Doxyfile.dev

clean:
	$(MAKE) -C test/ clean
	rm -Rf build doc

.PHONY: clean test junit docs