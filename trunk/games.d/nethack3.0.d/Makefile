#	Hack Makefile.
#	SCCS Id: @(#)Makefile.top	3.0	89/03/23

# make NetHack
GAME     = nethack
GAMEUID  = sat
GAMEGRP  = r386

#	GAMEDIR also appears in config.h as "HACKDIR".
GAMEDIR  = /usr/sat/games/$(GAME).lib
SHELLDIR = /usr/sat/games/shell
MANDIR   = /usr/sat/games/$(GAME).lib/man/manl/man6
MANEXT   = 6
# manual installation for most BSD-style systems
GAMEMANCREATE = cp auxil/nhack.6
LEVMANCREATE = cp auxil/lev_comp.6
# manual installation for most SYSV-style systems
# GAMEMANCREATE = nroff -man auxil/nhack.6 >
# LEVMANCREATE = nroff -man auxil/lev_comp.6 >

AUXHELP = help hh cmdhelp history opthelp oracles
VARAUX = data rumors
SPEC_LEVS = endgame castle tower1 tower2 tower3
AUX = $(AUXHELP) $(VARAUX) $(SPEC_LEVS) license

$(GAME):
	( cd src ; make )

all:	$(GAME) Guidebook data rumors spec_levs
	@echo "Done."

Guidebook:
	( cd auxil ; make Guidebook )

data:
	( cd auxil ; make data )

rumors:
	( cd auxil ; make rumors )

spec_levs:
	( cd src ; make lev_comp )
	( cd auxil ; make spec_levs )
	
update: $(GAME) data rumors spec_levs
# copy over new versions of the game files
	( cd auxil ; cp $(AUX) $(GAMEDIR) )
#	(don't yank the old version out from under people who're playing it)
	mv $(GAMEDIR)/../$(GAME) $(GAMEDIR)/../$(GAME).old
	cp src/$(GAME) $(GAMEDIR)/../$(GAME)
	cp auxil/$(GAME).sh $(SHELLDIR)/$(GAME)
# correct the permissions on the new versions
	-( cd $(GAMEDIR) ; chown $(GAMEUID) $(AUX) $(GAME) ; \
			chgrp $(GAMEGRP) $(AUX) $(GAME) ; \
			chmod 644 $(AUX) )
	-chown $(GAMEUID) $(SHELLDIR)/$(GAME)
	chgrp $(GAMEGRP) $(SHELLDIR)/$(GAME)
	chmod 0755 $(SHELLDIR)/$(GAME)
	chmod 04755 $(GAMEDIR)/../$(GAME)
# and touch time-sensitive files
	-touch -c $(GAMEDIR)/bones* $(GAMEDIR)/?lock* $(GAMEDIR)/wizard*
	-touch -c $(GAMEDIR)/save/*
	touch $(GAMEDIR)/perm $(GAMEDIR)/record

install: $(GAME) data rumors spec_levs
# set up the directories
	-mkdir $(SHELLDIR)
	-rm -rf $(GAMEDIR)
	-mkdir $(GAMEDIR) $(GAMEDIR)/save
	-mkdir $(GAMEDIR)/man $(GAMEDIR)/man/manl $(GAMEDIR)/man/manl/man6
# create some files
	touch $(GAMEDIR)/perm $(GAMEDIR)/record $(GAMEDIR)/logfile
# copy over the game files
	( cd auxil ; cp $(AUX) $(GAMEDIR) )
	cp src/$(GAME) $(GAMEDIR)/../$(GAME)
	cp auxil/$(GAME).sh $(SHELLDIR)/$(GAME)
# set up the permissions
	-chown $(GAMEUID) $(SHELLDIR)/$(GAME) $(GAMEDIR) $(GAMEDIR)/*
	chgrp $(GAMEGRP) $(SHELLDIR)/$(GAME) $(GAMEDIR) $(GAMEDIR)/*
	chmod 0755 $(SHELLDIR)/$(GAME)
	chmod 644 $(GAMEDIR)/*
	chmod 755 $(GAMEDIR) $(GAMEDIR)/save $(GAMEDIR)/man
	chmod 04755 $(GAMEDIR)/../$(GAME)
# and copy the man pages
	-$(GAMEMANCREATE) $(MANDIR)/$(GAME).$(MANEXT)
	-$(LEVMANCREATE) $(MANDIR)/lev_comp.$(MANEXT)

clean:
	( cd src ; make clean)

spotless:
	( cd src ; make spotless)