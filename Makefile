CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++2a

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

%.o: %.cpp %.d %.hpp
	$(CXX) -MT $@ -MMD -MP -MF $*.d $(CFLAGS) -c $(OUTPUT_OPTION) $<
$(DEPS):
include $(wildcard $(DEPS))
# %.o: %.cpp
# 	$(CXX) $(CXXFLAGS) -c $< -o $@

runtcp: all
	./$(TARGET) -h 0.0.0.0 -p 2023 -m tcp

runudp: all
	./$(TARGET) -h 0.0.0.0 -p 2023 -m udp

clean:
	rm -f $(OBJS) $(TARGET) $(DEPS)