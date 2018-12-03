SOURCES = $(shell find ./ -name '*.cc' | sort -k 1nr | cut -f2-)
OBJECTS = $(SOURCES:%.cc=%.o)
DEFINES ?=

all: $(OBJECTS)
	g++ -std=c++11 $(OBJECTS) -o run

%.o: %.cc
	g++ -std=c++11 -c $< -o $@ $(DEFINES)

clean:
	@rm -f *.o run
