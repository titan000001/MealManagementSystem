# Compiler and flags
CXX = g++
# CXXFLAGS specifies compiler options: C++17 standard, include path for mysql connector, all warnings, and debug symbols.
CXXFLAGS = -std=c++17 -I/usr/include/mysql-cppconn-8/ -Wall -Wextra -g
# LDFLAGS specifies linker options, linking against the required libraries.
LDFLAGS = -lmysqlcppconn -lssl -lcrypto

# The name of the final executable
TARGET = MealManagementSystem

# Directories
SRC_DIR = src
OBJ_DIR = obj

# Find all .cpp files in the source directory
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
# Generate corresponding object file names in the obj/ directory
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# The 'all' target is the default. It depends on the final executable.
all: $(TARGET)

# Rule to link the executable from all the object files
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Rule to compile a .cpp source file into a .o object file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR) # Create the obj directory if it doesn't exist
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to clean up all build artifacts
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Phony targets are not actual files.
.PHONY: all clean