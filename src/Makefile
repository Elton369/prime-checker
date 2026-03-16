CXX := g++
CXXFLAGS := -Wall -Wextra -Wpedantic -std=c++17 -O2
TARGET := ../usr/bin/prime-checker
SRC := main.cpp

all: check_dirs check_compiler $(TARGET)

check_dirs:
	@mkdir -p ../usr/bin

check_compiler:
	@command -v $(CXX) >/dev/null 2>&1 || { \
		echo "Error: compiler $(CXX) not found."; \
		exit 1; \
	}

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

rebuild: clean all