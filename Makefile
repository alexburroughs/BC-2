TARGET_EXEC ?= comp

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

CC = gcc

SRCS := $(shell find $(SRC_DIRS) -name *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
LDFLAGS := -lm -luuid

CFLAGS ?= $(INC_FLAGS) -MMD -MP --std=c99 -luuid
# List of external libraries.

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	gcc $(OBJS) -o $@ $(LDFLAGS)

# c++ source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	gcc $(CFLAGS) $(LIBS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
