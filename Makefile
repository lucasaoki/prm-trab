BINDIR = bin
INCDIR = /usr/local/include/player-3.0
LIBDIR = /usr/local/lib64
ODIR = obj
OUTDIR = out
SRCDIR = src
CC = g++


# CFLAGS = -I$(INCDIR) -g
CFLAGS = -I$(INCDIR) -Iinclude

LFLAGS = -L$(LIBDIR) $(LIBS)
LIBS = -lplayerc -lm `pkg-config opencv --cflags opencv --libs opencv`

$(ODIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(ODIR)
	mkdir -p $(OUTDIR)
	$(CC) -c -o $@ $< $(CFLAGS)

all: prm-trab

prm-trab: $(ODIR)/main.o $(ODIR)/Robot.o $(ODIR)/LocalMap.o
	$(CC) -o $(BINDIR)/$@ $^ $(LFLAGS)
    
.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(OUTDIR)/* $(BINDIR)/*
