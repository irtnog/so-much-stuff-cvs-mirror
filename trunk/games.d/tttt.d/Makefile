#
#	tttt $Header: /home/Vince/cvs/games.d/tttt.d/Makefile,v 1.7 2002-11-24 02:57:40 Vincent Exp $
#
ALL	= tttt$(EXE)
MODEL	= -i -F 2000
OPTIONS	= -O
include $(HOME)/CONFIG
CFILES	= rain.c

lint: $(CFILES) 
	lint -h $(CFILES) $(LIBS) 

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp tttt.6 $(MAN6)
	cd $(MAN6); chmod 644 tttt.6

clean:
	rm -f $(ALL) make.out core
