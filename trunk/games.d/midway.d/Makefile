#
#	Midway $Header: /home/Vince/cvs/games.d/midway.d/Makefile,v 1.5 2002-11-23 04:09:41 Vincent Exp $
#
# Leave in the -O blast you!
ALL	= Midway$(EXE)
MODEL	= -Mm
OPTIONS	= -O -DGAMLIB=$(GAMLIB)
include $(HOME)/CONFIG
FILES	= airstrike.c etc.c midway.c movebombs.c moveships.c screen.c
OBJS	= airstrike.o etc.o midway.o movebombs.o moveships.o screen.o
OTHERF	= makefile externs.h globals.h Midway.doc
JUNKF	= fluff junk arch tags

Midway$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

arch:
	ar uv MID.a $(FILES) $(OTHERF) 
	touch arch

profile: $(OBJS)
	cc -p -i $(OBJS) $(CFLAGS) $(LIBS) -o Midway.pro

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	touch $(GAMDIR)/.midwaylog
	chmod 666 $(GAMDIR)/.midwaylog
	cp Midway.doc $(GAMDIR)
	chmod 644 $(GAMDIR)/Midway.doc

clean:
	rm -f $(ALL) $(OBJS) $(JUNKF)

$(OBJS): externs.h
midway.o: globals.h
