.PHONY: all
all: hello-check-xform

# **** configure

STOPWATCH := ../../Pork/stopwatch
MCPP := $(HOME)/mcpp/src/mcpp

# **** clean

.PHONY: clean
clean:
	rm -f hello hello_stopwatch *.ii *.diff *_stopwatch.cpp

# **** source to source transform

hello.ii: hello.cpp
# oddly, mcpp is NOT working, whereas g++ seems to work fine
#	g++ -Wall -E $< > $@
	$(MCPP) -K $< > $@

hello_stopwatch.diff: hello.ii
# the stopwatch --methodlist argument is not working; no matter
# 	$(STOPWATCH) --methodlist hello_methods.txt $<
	$(STOPWATCH) $< > $@

hello_stopwatch.cpp: hello_stopwatch.diff
# NOTE: this patches the ORIGINAL file, not the .ii file!
	patch < $< -o $@

.PHONY: hello-check-xform
hello-check-xform: hello_stopwatch.cpp
	diff -u hello.cpp $<

# **** compile

# Note that we can't compile the transformed file because it refers to
# a header file that is not included in Pork.

hello_stopwatch: hello_stopwatch.cpp
	@echo
	@echo "This will fail because stopwatch inserts a ref to instrument.h"
	@echo
	g++ -Wall -o $@ $^

hello: hello.ii
	g++ -Wall -o $@ $^

# **** test

.PHONY: test
test:
	@echo "hello"
	./hello
	@echo; echo "hello_stopwatch"
	./hello_stopwatch
