#
# $Header: /home/Vince/cvs/games.d/fortune.d/Makefile,v 1.6 2002-11-24 20:08:08 vrs Exp $
#
# Makefile for "fortune" program.  See READ_ME for details.
#
ALL	= fortune$(EXE) fortunes.dat
OPTIONS	= -O -DNO_RANDOM -DFORTFILE=\"$(GAMLIB)/fortunes.dat\"
MODEL	= -i
include $(HOME)/CONFIG
OFILES	= fortune.o strfile.o rnd.o unstr.o
CFILES	= $(OFILES:.o=.c) strfile.h

fortune$(EXE): fortune.o rnd.o
	$(CC) $(CFLAGS) -o $@ fortune.o rnd.o $(LIBS)

fortunes.dat: strfile$(EXE) fortunes
	./strfile$(EXE) -r fortunes

strfile$(EXE): strfile.o rnd.o
	$(CC) $(CFLAGS) -o $@ strfile.o rnd.o

fortune.o strfile.o: strfile.h

fortunes: scene obscene
	cp scene fortunes
	chmod u+w fortunes
	echo "%-" >> fortunes
	cat obscene >> fortunes

unstr$(EXE): untsr.o
	$(CC) $(CFLAGS) -o $@ unstr.o

unstr.o: unstr.c strfile.h

install: $(ALL) fortunes.dat $(GAMDIR) $(GAMLIB)
	cp fortune$(EXE) $(GAMDIR)/
	cd $(GAMDIR); chown $(SAFEUID) fortune$(EXE); chmod 4711 fortune$(EXE)
	cp fortunes.dat $(GAMLIB)/
	cd $(GAMLIB); chown $(SAFEUID) fortunes.dat; chmod 600 fortunes.dat

clean:
	rm -f $(ALL) fortunes strfile unstr$(EXE) core $(OFILES)
	rm -f Spell.hlist *.sp strfile$(EXE)
	rm -f Oscene Oobscene scene.dat obscene.dat

lint:
	lint -hxb $(DEFS) fortune.c rnd.c 2>&1 > fortune.lint
	lint -hxb $(DEFS) strfile.c rnd.c 2>&1 > strfile.lint
	lint -hxb $(DEFS) unstr.c 2>&1 > unstr.lint

tags: $(CFILES)
	ctags $(CFILES)

spell:	scene.sp obscene.sp

Spell.hlist: scene.sp.ok obscene.sp.ok
	cat scene.sp.ok obscene.sp.ok | spellin /usr/dict/hlista > Spell.hlist

scene.sp: Spell.hlist scene
	spell -d Spell.hlist scene > scene.sp

obscene.sp: Spell.hlist obscene
	spell -d Spell.hlist obscene > obscene.sp

done_spell:
	sort -u -o scene.sp.ok scene.sp.ok scene.sp
	sort -u -o obscene.sp.ok obscene.sp.ok obscene.sp

troff: troff.scene troff.obscene

troff.scene:
	./Do_troff scene $(TROFF)

troff.obscene:
	./Do_troff obscene $(TROFF)

sort: sort.scene sort.obscene

sort.scene: strfile unstr$(EXE)
	strfile -oi scene
	mv scene Oscene
	./unstr$(EXE) -o scene

sort.obscene: strfile unstr$(EXE)
	strfile -oi obscene
	mv obscene Oobscene
	./unstr$(EXE) -o obscene
