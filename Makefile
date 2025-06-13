CC = gcc
CFLAGS = -fPIC -O2 -Wall -Wextra
DEBUG_CFLAGS = -fPIC -g -O0 -DDEBUG -Wall -Wextra -fsanitize=address -fsanitize=undefined
INCLUDES = -I src/
BUILDDIR = build
SRCDIR = src

# Source files
MAIN_SRCS = $(SRCDIR)/main.c $(SRCDIR)/parser.c $(SRCDIR)/dpll.c $(SRCDIR)/formula.c $(SRCDIR)/discovery.c
PLUGIN_SRCS = $(SRCDIR)/plugin/simple.c

# Targets
MAIN_TARGET = $(BUILDDIR)/main.o
PLUGIN_TARGET = $(BUILDDIR)/libimplement.so
DEBUG_MAIN_TARGET = $(BUILDDIR)/main_debug.o
DEBUG_PLUGIN_TARGET = $(BUILDDIR)/libimplement_debug.so

.PHONY: all clean debug

all: $(MAIN_TARGET) $(PLUGIN_TARGET)

debug: $(DEBUG_MAIN_TARGET) $(DEBUG_PLUGIN_TARGET)

$(BUILDDIR):
	@echo "Creating build directory"
	@mkdir -p $@

$(MAIN_TARGET): $(MAIN_SRCS) | $(BUILDDIR)
	@echo "Building main"
	$(CC) $(CFLAGS) -export-dynamic $^ $(INCLUDES) -o $@

$(PLUGIN_TARGET): $(PLUGIN_SRCS) | $(BUILDDIR)
	@echo "Building plugin"
	$(CC) $(CFLAGS) -shared $(INCLUDES) -o $@ $^

$(DEBUG_MAIN_TARGET): $(MAIN_SRCS) | $(BUILDDIR)
	@echo "Building main (debug)"
	$(CC) $(DEBUG_CFLAGS) -export-dynamic $^ $(INCLUDES) -o $@

$(DEBUG_PLUGIN_TARGET): $(PLUGIN_SRCS) | $(BUILDDIR)
	@echo "Building plugin (debug)"
	$(CC) $(DEBUG_CFLAGS) -shared $(INCLUDES) -o $@ $^

clean:
	rm -rf $(BUILDDIR)/
