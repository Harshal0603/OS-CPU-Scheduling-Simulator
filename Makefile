CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
SRC = src/main.cpp src/Scheduler.cpp src/GanttChart.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = scheduler

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

# Rebuild object files when any header changes too.
%.o: %.cpp $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(TARGET)
	sh tests/run_tests.sh

clean:
	rm -f $(OBJ) $(TARGET) scheduler.exe

.PHONY: all test clean
