.PHONY: all clean

CXX = g++
CXXFLAGS = -g -Wall -Wextra -Wuninitialized -O2 -std=c++11 -Isrc
SOURCES = $(wildcard src/*.cc)
MODULES := src/formats src/mzrtp
-include $(patsubst %, %/module.mk, $(MODULES))
OBJECTS := $(patsubst %.cc, %.o, $(filter %.cc, $(SOURCES)))

TARGET = gabriel

all: $(TARGET)

src/%.o: src/%.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TARGET): $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS)

clean:
	rm -f src/*.o $(TARGET)
