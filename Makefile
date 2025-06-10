
CC = gcc
CFLAGS = -fPIC -O2
INCLUDES = -I src/
BUILDDIR = build
SRCDIR = src

# Source files
MAIN_SRCS = $(SRCDIR)/main.c $(SRCDIR)/parser.c $(SRCDIR)/dpll.c $(SRCDIR)/formula.c $(SRCDIR)/discovery.c
PLUGIN_SRCS = $(SRCDIR)/plugin/plugin4.c

# Targets
MAIN_TARGET = $(BUILDDIR)/main.o
PLUGIN_TARGET = $(BUILDDIR)/libimplement.so

.PHONY: all clean

all: $(MAIN_TARGET) $(PLUGIN_TARGET)

$(BUILDDIR):
	@echo "Creating build directory"
	@mkdir -p $@

$(MAIN_TARGET): $(MAIN_SRCS) | $(BUILDDIR)
	@echo "Building main"
	$(CC) $(CFLAGS) -export-dynamic $^ $(INCLUDES) -o $@

$(PLUGIN_TARGET): $(PLUGIN_SRCS) | $(BUILDDIR)
	@echo "Building plugin"
	$(CC) $(CFLAGS) -shared $(INCLUDES) -o $@ $^

clean:
	rm -rf $(BUILDDIR)/
