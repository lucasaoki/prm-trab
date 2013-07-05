BINDIR = bin
INCDIR = /usr/local/include/player-3.0
OBJDIR = obj
OUTDIR = out
SRCDIR = src
CC = g++

LBITS := $(shell getconf LONG_BIT)
ifeq ($(LBITS),64)
	LIBDIR = /usr/local/lib64
else
	LIBDIR = /usr/local/lib
endif


# CFLAGS = -I$(INCDIR) -g
CFLAGS = -I$(INCDIR) -Iinclude

LFLAGS = -L$(LIBDIR) $(LIBS)
LIBS = -lplayerc -lm `pkg-config opencv --cflags opencv --libs opencv`

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(OBJDIR)
	$(CC) -c -o $@ $< $(CFLAGS)

all: prm-trab roboreal

prm-trab: $(OBJDIR)/main.o $(OBJDIR)/Robot.o $(OBJDIR)/LocalMap.o
	$(CC) -o $(BINDIR)/$@ $^ $(LFLAGS)

roboreal: $(OBJDIR)/main.o $(OBJDIR)/roboReal.o $(OBJDIR)/LocalMap.o
	$(CC) -o $(BINDIR)/$@ $^ $(LFLAGS)
    
.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o *~ core $(OUTDIR)/* $(BINDIR)/*
