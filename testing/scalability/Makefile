# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra

# Source files
SRCS = client.cpp ../../docker-deploy/src/server_util.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Output executable
OUTPUT = client

# Rule to compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to link object files into the executable
$(OUTPUT): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(OUTPUT)
