#
#	reversi @(#)Makefile	1.3 4/8/86
#
MODEL	= -i -F 2000
ALL	= reversi$(EXE)
include $(HOME)/CONFIG
YFLAGS	= -dv
OFILES	= user.o ulex.o move.o display.o hasmove.o fini.o \
	  minmax.o score.o edges.o corners.o count.o
SOURCE	= Makefile corners.c count.c display.c edges.c fini.c genedge.c \
	  hasmove.c makeedge.y minmax.c move.c reversi.h score.c ulex.l user.y

reversi$(EXE): $(OFILES)
	$(CC) $(CFLAGS) -o $@ $(OFILES) $(LIBS)

reversi.shar: $(SOURCE)
	shar -c $(SOURCE) > reversi.shar

$(OFILES): reversi.h

edges.o: edges.c edges.out
	$(CC) $(CFLAGS) -c edges.c
edges.out: makeedge genedge$(EXE)
	./genedge$(EXE) | ./makeedge$(EXE) > edges.out

makeedge.c: makeedge.y
	$(YACC) makeedge.y
	mv y.tab.c makeedge.c

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)

clean:
	rm -f *.o makeedge.c ulex.c user.c makeedge.c
	rm -f genedge$(EXE) makeedge$(EXE) reversi$(EXE)
	rm -f core make.out reversi.shar edges.out y.output y.tab.h
