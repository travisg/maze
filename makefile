CFLAGS := -Wall -Wextra -O
SRCS := main.cpp
TARGET := maze
DEPS := $(TARGET).d

all: maze

$(TARGET): $(SRCS)
	$(CXX) $(CFLAGS) $(SRCS) -MD -MT $@ -MF $@.d -o $@

clean:
	rm -f -- $(TARGET) $(DEPS)

.PHONY: all clean

ifeq ($(filter $(MAKECMDGOALS), clean), )
-include $(DEPS)
endif