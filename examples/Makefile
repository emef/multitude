CXX := g++
CXXFLAGS := -std=c++14 -Wall -O3
LIBFLAGS := -L../lib -lmultitude

bindir := bin/
srcs := $(shell ls ./*.cc)
objects  := $(patsubst %.cc, %.o, $(srcs))
executables := $(addprefix $(bindir),$(patsubst %.cc, %, $(srcs)))

all: $(executables)

$(executables): $(objects)
	$(CXX) $(CXXFLAGS) $(LIBFLAGS) -o $@ $<

depend: .depend

.depend: $(srcs)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

clean:
	rm -rf $(bindir)
	rm -f $(objects)

dist-clean: clean
	rm -f *~ .depend

include .depend
