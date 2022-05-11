BASE_DIR := .
SOURCE_DIR := ./source
INCLUDE_DIR := ./include
DEBUG_DIR := $(SOURCE_DIR)/debug

DEBUG := 0
O_LEVEL := g
C_FLAGS := -I$(INCLUDE_DIR) -O$(O_LEVEL) -Wall -Wextra -Wfatal-errors
ifeq ($(DEBUG), 1)
	C_FLAGS += -g -DDEBUG=1
endif
CXX_FLAGS := $(C_FLAGS)

SRC_FILES :=$(wildcard $(SOURCE_DIR)/*.cpp)

ifeq ($(DEBUG), 1)
	SRC_FILES +=$(wildcard $(DEBUG_DIR)/*.cpp)
endif

TARGET = lc3-simulator

all: $(TARGET)

.PHONY: all clean

$(TARGET): $(SRC_FILES)
	$(echo $(SRC_FILES))
	g++ $(CXX_FLAGS) $(SRC_FILES) -o $(TARGET)

clean:
	@$(RM) $(TARGET)

