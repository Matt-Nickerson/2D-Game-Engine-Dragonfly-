

# Toolchain
CXX      ?= g++
AR       ?= ar
RM       ?= rm -f
MKDIR_P  ?= mkdir -p

# Project layout
SRC_DIR  := src
INC_DIR  := include
BUILD_DIR:= build
BIN_DIR  := bin

# Output
TARGET   := $(BIN_DIR)/dragonfly

# Sources/Objects
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Compiler/Linker flags
CXXFLAGS += -std=gnu++17 -Wall -Wextra -Wpedantic -O2 -I$(INC_DIR)

# If your code uses fopen_s (MSVC), make it work on g++/clang too:
#   fopen_s(&fp, "file", "w")  -> sets fp via fopen, returns 0 on success, 1 on failure
CXXFLAGS += -Dfopen_s\(fp,filename,mode\)=((*(fp)=fopen((filename),(mode)))==nullptr?1:0)

LDFLAGS  :=

# ---- Targets ----
.PHONY: all run clean debug release

all: $(TARGET)

debug: CXXFLAGS += -O0 -g
debug: clean all

release: CXXFLAGS += -O3 -DNDEBUG
release: clean all

run: $(TARGET)
	@echo "Running $(TARGET)..."
	@$(TARGET)

clean:
	$(RM) -r $(BUILD_DIR) $(BIN_DIR)

# Link
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Built $@"

# Compile
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure dirs exist
$(BUILD_DIR):
	$(MKDIR_P) $(BUILD_DIR)

$(BIN_DIR):
	$(MKDIR_P) $(BIN_DIR)
