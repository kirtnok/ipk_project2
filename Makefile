CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20

# Get all .cpp files
SRCS = $(wildcard *.cpp)
# Get corresponding .o files
OBJS := $(SRCS:%.cpp=%.o)
# Get corresponding .d files
DEPS := $(SRCS:%.cpp=%.d)

TARGET = ipkcpd

.phony: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp %.d %.h
	$(CXX) -MT $@ -MMD -MP -MF $*.d $(CXXFLAGS) -c $(OUTPUT_OPTION) $<
$(DEPS):
include $(wildcard $(DEPS))

runtcp: all
	./$(TARGET) -h 0.0.0.0 -p 2023 -m tcp

runudp: all
	./$(TARGET) -h 0.0.0.0 -p 2023 -m udp

clean:
	rm -f $(OBJS) $(TARGET) $(DEPS)