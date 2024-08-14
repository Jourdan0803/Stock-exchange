# Makefile for C++ Server Project

# Compiler settings
CXX = g++
CXXFLAGS = -I/usr/include/pqxx -Wall -std=c++11 -pthread 
EXTRAFLAGS=-lpqxx -lpq 

# Define the executable name
TARGET = server

# Source files
SOURCES = server.cpp server_util.cpp parser.cpp database_util.cpp xml_parser/pugixml.cpp
HEADERS = server_util.hpp parser.hpp database_util.h

# Build rule
all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES) $(EXTRAFLAGS) 

# Clean rule
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean