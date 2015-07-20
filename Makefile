
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