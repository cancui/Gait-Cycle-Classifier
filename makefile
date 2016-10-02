BIN = main.exe
CC = g++
CFLAGS = -Wall -Wextra -std=c++11
BUILDDIR = build

#Add other objects here
OBJS = $(addprefix $(BUILDDIR)/, main.o feature_extractor.o gait_cycle_classifier.o csvparser.o) 

all: $(BUILDDIR) $(BUILDDIR)/$(BIN)

$(BUILDDIR)/$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

#$(BUILDDIR)/csvparser.o: csvparser.c
#	$(CC) $(CFLAGS) -o $@ -c $^

$(BUILDDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $^


$(BUILDDIR):
	mkdir $(BUILDDIR)

run:
	$(BUILDDIR)/$(BIN)

clean:
	rm -f $(BUILDDIR)/*.o