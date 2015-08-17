CXX := clang++
CXXFLAGS := -std=c++14 -Wall -O3

libdir := lib/
srcs := $(shell ls ./*.cc)
objects  := $(patsubst %.cc, %.o, $(srcs))
lib := $(libdir)libmultitude.a

$(info lib is $(lib))

all: $(libdir) $(lib)

$(libdir):
	mkdir -p $(libdir)

$(lib): $(objects)
	ar ru $@ $^
	ranlib $@

examples: $(lib)
	make -C examples

tools: $(lib)
	make -C tools

depend: .depend

.depend: $(srcs)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

clean:
	rm -f $(objects) $(lib)
	make -C examples clean
	make -C tools clean

dist-clean: clean
	rm -f *~ .depend
	make -C examples dist-clean
	make -C tools dist-clean

include .depend
