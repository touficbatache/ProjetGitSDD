CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lm

SRCDIR = .
OBJDIR = obj
BINDIR = .

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

TARGETS = $(BINDIR)/lit $(BINDIR)/litTest

all: $(TARGETS)

$(BINDIR)/lit: $(OBJDIR)/lit.o $(OBJDIR)/litCore.o
	$(CC) $(CFLAGS) $^ -o $@

$(BINDIR)/litTest: $(OBJDIR)/litTest.o $(OBJDIR)/litCore.o
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGETS)

-include $(DEPS)