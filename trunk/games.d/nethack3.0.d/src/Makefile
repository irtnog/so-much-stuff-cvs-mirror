#	Hack Makefile.
#	SCCS Id: @(#)Makefile.unix	3.0	88/10/17
MODEL	= -Ml -Gt24
OPTIONS	= -O -I../include -DHACKDIR=\"$(GAMLIB)/nethack3.0\" #$(WSTRINGS)
GAME	= nethack3.0
ALL		= $(GAME)
include $(HOME)/CONFIG
# This makefile replaces the previous Makefile.unix, Makefile.xenix,
# Makefile.3B2, Makefile.att, and Makefile.tos.
# Set SYSTEM to one of:
#	'Sysunix'	-- generic UNIX
#	'Sysxenix'	-- 286 Xenix (386 Xenix can use Sysunix)
#	'Sys3B2'	-- AT&T 3B2, 3B5, etc.
#	'Sysatt'	-- AT&T UNIXPC, 7300, 3B1
#	'Systos'	-- Atari
#	'SysV-AT'	-- Microport 286 UNIX (put -DDUMB in CFLAGS)
SYSTEM = Sysunix

#
# Make sure that your bourne shell is specified here, as you have to spawn
# some of the commands (eg. depend) in bourne shell for them to work.
#
SHELL=/bin/sh

# if you are cross-compiling (eg. from Xenix into a Dos enviornment) you will
# have to redefine this filename prefix (which is used to select the filenames
# *main.c, *tty.c, and *unix.c for the target environment)
#
# possible values are 'unix' and 'pc'
# Systos wants 'pc'; everything else normally wants 'unix'
TARG = unix

# object files for makedefs
MAKEOBJS = makedefs.o monst.o objects.o panic.o
# objects files for makedefs for 286 Xenix
# MAKEOBJS = Smakedefs.o Smonst.o Sobjects.o Spanic.o

# object files for special levels compiler
SPLEVOBJS = lev_comp.o lev_lex.o lev_main.o alloc.o monst.o objects.o panic.o
# object files for special levels compiler for 286 Xenix
# SPLEVOBJS = Slev_comp.o Slev_lex.o Slev_main.o Salloc.o Smonst.o Sobjects.o \
#	      Spanic.o

# on some systems the termcap library is in -ltermcap or -lcurses
# Sysatt uses shared library in lieu of this option
# TERMLIB = -ltermcap
# TERMLIB = -lcurses
# TERMLIB = -ltermlib
#T_LIB_DIR = /usr/sat/c_src/lib
#TERMLIB = $(T_LIB_DIR)/gnutcap.o $(T_LIB_DIR)/sysdep.o $(T_LIB_DIR)/tparam.o
TERMLIB	= $(LIBS)

# yacc/lex programs to use to generate lev_comp.c, lev_comp.h, and lev_lex.c.
# if, instead of yacc/lex you have bison/flex, comment/uncomment the following.
#YACC     = yacc
#LEX      = lex
#YACC     = bison
#LEX      = flex

# if you defined RANDOM in unixconf.h/pcconf.h since your system did not come
# with a reasonable random number generator -- also remember to get random.c
# into the src directory
# RANDOBJ = random.o
RANDOBJ =

# nothing below this line should have to be changed
#
# other things that have to be reconfigured are in config.h,
# {unixconf.h, pcconf.h, tosconf.h}, and possibly system.h

HACKCSRC = alloc.c apply.c artifact.c attrib.c bones.c cmd.c dbridge.c decl.c\
	   demon.c do.c do_name.c do_wear.c dog.c dogmove.c dokick.c dothrow.c\
 	   eat.c end.c engrave.c exper.c extralev.c fountain.c getline.c hack.c\
	   invent.c ioctl.c lock.c mail.c makemon.c mcastu.c mhitm.c mhitu.c\
	   mklev.c mkmaze.c mkobj.c mkroom.c mon.c mondata.c monmove.c monst.c\
	   mthrowu.c music.c o_init.c objects.c objnam.c options.c pager.c\
	   pickup.c polyself.c potion.c pray.c pri.c priest.c prisym.c read.c\
 	   restore.c rip.c rnd.c rumors.c save.c search.c shk.c shknam.c sit.c\
	   sounds.c sp_lev.c spell.c steal.c termcap.c timeout.c topl.c topten.c\
	   track.c trap.c u_init.c uhitm.c vault.c version.c weapon.c were.c\
	   wield.c wizard.c worm.c worn.c write.c zap.c

# all .c files but msdos.c, tos.c, *main.c, *tty.c, *unix.c, (system specific)
# and makedefs.c, lev_comp.c, panic.c (not part of any nethack)

CSOURCES = $(HACKCSRC) $(TARG)main.c $(TARG)main.c $(TARG)main.c makedefs.c panic.c

HACKINCL = artifact.h attrib.h config.h coord.h decl.h edog.h epri.h eshk.h\
	   extern.h flag.h func_tab.h global.h gold.h hack.h lev.h mfndpos.h\
	   mkroom.h monattk.h mondata.h monflag.h monst.h monsym.h msdos.h\
	   obj.h objclass.h permonst.h prop.h rm.h sp_lev.h spell.h tradstdc.h\
	   trapname.h vault.h wseg.h you.h youprop.h

# all .h files except date.h, onames.h, pm.h & trap.h which would cause
# dependency loops if run through "make depend".

HSOURCES = $(HACKINCL) date.h onames.h pm.h trap.h

SOURCES = $(CSOURCES) $(HSOURCES)

HOBJ = alloc.o apply.o artifact.o attrib.o bones.o cmd.o dbridge.o decl.o\
	demon.o do.o do_name.o do_wear.o dog.o dogmove.o dokick.o dothrow.o\
 	eat.o end.o engrave.o exper.o extralev.o fountain.o getline.o hack.o\
	invent.o ioctl.o lock.o mail.o main.o makemon.o mcastu.o mhitm.o\
	mhitu.o mklev.o mkmaze.o mkobj.o mkroom.o mon.o mondata.o monmove.o\
	monst.o mthrowu.o music.o o_init.o objects.o objnam.o options.o pager.o\
	pickup.o polyself.o potion.o pray.o pri.o priest.o prisym.o read.o\
	restore.o rip.o rnd.o rumors.o save.o search.o shk.o shknam.o sit.o\
	sounds.o sp_lev.o spell.o steal.o termcap.o timeout.o topl.o topten.o\
	track.o trap.o tty.o u_init.o uhitm.o unix.o vault.o version.o\
	weapon.o were.o wield.o wizard.o worm.o worn.o write.o zap.o $(RANDOBJ)

# the .o files from the HACKCSRC list, plus main.o tty.o unix.o

$(GAME):	$(SYSTEM)
	@echo "$(GAME) is up to date."

Sysunix:	$(HOBJ) Makefile
	@echo "Loading ..."
	@$(CC) $(LFLAGS) -o $(GAME) $(HOBJ) $(TERMLIB)
	@touch Sysunix

Sysxenix:	$(HOBJ) Makefile
	@echo "Loading ..."
	@$(CC) $(LFLAGS) -m hack.map -o $(GAME) /lib/Lsignal.o $(HOBJ) $(TERMLIB) -SEG#256
	@touch Sysxenix

Sys3B2:	$(HOBJ) Makefile
	@echo "Loading ..."
	@$(CC) $(LFLAGS) -o $(GAME) $(HOBJ) $(TERMLIB) -lmalloc
	@touch Sys3B2

Sysatt:	$(HOBJ) Makefile
	@echo "Loading ..."
	@$(LD) $(LFLAGS) /lib/crt0s.o /lib/shlib.ifile -o $(GAME) $(HOBJ)
	@touch Sysatt

Systos:	$(HOBJ) tos.o Makefile
	@echo "Loading ..."
	@$(CC) $(LFLAGS) -o $(GAME) $(HOBJ) tos.o $(TERMLIB)
	@touch Systos

SysV-AT:	DUMB.Setup $(HOBJ) Makefile
	@echo "Loading ..."
	@$(CC) $(LFLAGS) -o $(GAME) $(HOBJ) $(TERMLIB)
	@touch SysV-AT

DUMB.Setup:	../include/extern.h
	cp ../include/extern.h ../include/extern.h.BAK 
	cat ../include/extern.h | \
		sed -e '/^E\ int\ /!b' \
			-e '/[^;/ 	]$$/N' \
			-e '/[(][*]occupation[)]/b' \
			-e '/[(][*]afternmv[)]/b' \
			-e '/float_down/b' \
			-e '/done1/b' \
			-e '/identify/b' \
			-e '/Hear_again/b' \
			-e '/hangup/b' \
			-e 's/^\(.*\)$$/\/\* \1 \/\*\*\//' | \
		sed -e '/^E\ void\ /!b' \
			-e '/[^;/ 	]$$/N' \
			-e 's/^\(.*\)$$/\/\* \1 \/\*\*\//' \
				>../include/extern.DUMB 
	cp ../include/extern.DUMB ../include/extern.h 
	@touch DUMB.Setup

all:	$(GAME)

#
#	Please note:	The dependency lines for the modules here are
#			deliberately incorrect.  Including "hack.h" in
#			the dependency list would cause a dependency
#			loop.
#
makedefs$(EXE):	$(MAKEOBJS)
	$(CC) $(LFLAGS) -o $@ $(MAKEOBJS)

makedefs.o:  ../include/config.h ../include/permonst.h

lev_comp$(EXE):  $(SPLEVOBJS)
	$(CC) $(LFLAGS) -o $@ $(SPLEVOBJS)

lev_comp.o:  ../include/hack.h ../include/sp_lev.h

lev_lex.o:   ../include/lev_comp.h ../include/hack.h ../include/sp_lev.h

lev_main.o:  ../include/hack.h ../include/sp_lev.h

../include/lev_comp.h: lev_comp.c

lev_comp.c: lev_comp.y
	$(YACC) -d lev_comp.y
	mv y.tab.c lev_comp.c
	mv y.tab.h ../include/lev_comp.h

lev_lex.c: lev_comp.l
	$(LEX) lev_comp.l
	mv lex.yy.c lev_lex.c

# special stuff for 286 Xenix makedefs & lev_comp

Smakedefs.o:	makedefs.c ../include/config.h ../include/permonst.h
	cp makedefs.c Smakedefs.c
	$(CC) -O -c Smakedefs.c

Slev_comp.o:  lev_comp.c ../include/hack.h ../include/sp_lev.h
	cp lev_comp.c Slev_comp.c
	$(CC) -O -c Slev_comp.c

Slev_lex.o: lev_lex.c ../include/hack.h ../include/sp_lev.h ../include/lev_comp.h
	cp lev_lex.c Slev_lex.c
	$(CC) -O -c Slev_lex.c

Slev_main.o: lev_main.c ../include/hack.h ../include/sp_lev.h
	cp lev_main.c Slev_main.c
	$(CC) -O -c Slev_main.c

Salloc.o:	alloc.c
	cp alloc.c Salloc.c
	$(CC) -O -c Salloc.c

Smonst.o:	monst.c ../include/config.h ../include/permonst.h \
		../include/monsym.h ../include/epri.h ../include/eshk.h \
		../include/vault.h
	cp monst.c Smonst.c
	$(CC) -O -c Smonst.c

Sobjects.o:	objects.c ../include/config.h ../include/obj.h \
		../include/objclass.h ../include/prop.h
	cp objects.c Sobjects.c
	$(CC) -O -c Sobjects.c

Spanic.o:	panic.c
	cp panic.c Spanic.c
	$(CC) -O -c Spanic.c

#
#	The following include files depend on makedefs to be created.
#	As a result, they are not defined in HACKINCL, instead, their
#	dependencies are explicitly outlined here.
#

#
#	date.h should be remade any time any of the source or include code
#	is modified.  Unfortunately, this would make the contents of this
#	file far more complex.  Since "hack.h" depends on most of the include
#	files, we kludge around this by making date.h dependent on hack.h,
#	even though it doesn't include this file.
#
../include/date.h:	$(CSOURCES) ../include/hack.h makedefs$(EXE)
	chmod u+w $@
	./makedefs -v

../include/trap.h:	../include/config.h makedefs$(EXE)
	chmod u+w $@
	./makedefs -t

../include/onames.h:	makedefs$(EXE)
	chmod u+w $@
	./makedefs -o

../include/pm.h:	makedefs$(EXE)
	chmod u+w $@
	./makedefs -p

#
#	The following programs vary depending on what OS you are using.
#	As a result, they are not defined in HACKSRC, and their dependancies
#	are explicitly outlined here.
#
main.o:	../include/hack.h $(TARG)main.c
	$(CC) $(CFLAGS) -c $(TARG)main.c
	mv $(TARG)main.o main.o

tty.o:	../include/hack.h $(TARG)tty.c
	$(CC) $(CFLAGS) -c $(TARG)tty.c
	mv $(TARG)tty.o tty.o

unix.o:	../include/hack.h $(TARG)unix.c
	$(CC) $(CFLAGS) -c $(TARG)unix.c
	mv $(TARG)unix.o unix.o

lint:
# lint cannot have -p here because (i) capitals are meaningful:
# [Ww]izard, (ii) identifiers may coincide in the first six places:
# doweararm() versus dowearring().
# _flsbuf comes from <stdio.h>, a bug in the system libraries.
	@echo lint -axbh -DLINT ...
	@lint -axbh -I../include -DLINT $(HACKCSRC) $(TARG)main.c $(TARG)tty.c $(TARG)unix.c | sed '/_flsbuf/d'


diff:
	@- for i in $(SOURCES) ; do \
		cmp -s $$i $D/$$i || \
		( echo diff $D/$$i $$i ; diff $D/$$i $$i ; echo ) ; done

tags: $(CSOURCES)
	@echo ctags -tw ...
	@ctags -tw $(CSOURCES)
	@( cd ../include ; ctags -tw $(HSOURCES) )
  
clean:
	rm -f *.o lev_comp.c ../include/lev_comp.h lev_lex.c
	rm -f lev_comp$(EXE) makedefs$(EXE) $(ALL)
	cd ../include; chmod a-w date.h onames.h pm.h trap.h

spotless: clean
	rm -f a.out core $(GAME) lev_comp$(EXE) makedefs
	rm -f ../include/onames.h ../include/pm.h
	touch ../include/onames.h ../include/pm.h
	touch makedefs.c
#(make sure files exist and have timestamps in the right order for next compile)


depend:
# For the moment we are lazy and disregard /usr/include files because
# the sources contain them conditionally. Perhaps we should use cpp.
#		( /bin/grep '^#[ 	]*include' $$i | sed -n \
#			-e 's,<\(.*\)>,"/usr/include/\1",' \
#
#
	for i in ${HACKCSRC}; do \
		( /bin/grep '^#[ 	]*include[ 	]*"' $$i | sed -n \
			-e 's/[^"]*"\([^"]*\)".*/\1/' \
			-e 's/.*\.h/..\/include\/&/' \
			-e H -e '$$g' -e '$$s/\n/ /g' \
			-e '$$s/.*/'$$i': &/' -e '$$s/\.c:/.o:/p' \
			>> makedep); done
	for i in ${HACKINCL}; do \
		( /bin/grep '^#[ 	]*include[ 	]*"' ../include/$$i | sed -n \
			-e 's/[^"]*"\([^"]*\)".*/\1/' \
			-e 's/.*\.h/..\/include\/&/' \
			-e H -e '$$g' -e '$$s/\n/ /g' \
			-e '$$s/.*/..\/include\/'$$i': &\
				touch ..\/include\/'$$i/p \
			>> makedep); done
	@echo '/^# DO NOT DELETE THIS LINE/+2,$$d' >eddep
	@echo '$$r makedep' >>eddep
	@echo 'w' >>eddep
	@cp Makefile Makefile.bak
	ed - Makefile < eddep
	@rm -f eddep makedep
	@echo '# DEPENDENCIES MUST END AT END OF FILE' >> Makefile
	@echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> Makefile
	@echo '# see make depend above' >> Makefile
	- diff Makefile Makefile.bak
	@rm -f Makefile.bak

# PC dependency for Systos that won't be made on a UNIX system
tos.o:	../include/hack.h ../include/osbind.h

# DO NOT DELETE THIS LINE

alloc.o:  ../include/config.h
apply.o:  ../include/hack.h ../include/edog.h
artifact.o:  ../include/hack.h ../include/artifact.h
attrib.o:  ../include/hack.h
bones.o:  ../include/hack.h
cmd.o:  ../include/hack.h ../include/func_tab.h
dbridge.o:  ../include/hack.h
decl.o:  ../include/hack.h
demon.o:  ../include/hack.h
do.o:  ../include/hack.h
do_name.o:  ../include/hack.h
do_wear.o:  ../include/hack.h
dog.o:  ../include/hack.h ../include/edog.h
dogmove.o:  ../include/hack.h ../include/mfndpos.h ../include/edog.h
dokick.o:  ../include/hack.h ../include/eshk.h
dothrow.o:  ../include/hack.h
eat.o:  ../include/hack.h
end.o:  ../include/hack.h ../include/eshk.h
engrave.o:  ../include/hack.h
exper.o:  ../include/hack.h
extralev.o:  ../include/hack.h
fountain.o:  ../include/hack.h
getline.o:  ../include/hack.h ../include/func_tab.h
hack.o:  ../include/hack.h
invent.o:  ../include/hack.h ../include/lev.h ../include/wseg.h
ioctl.o:  ../include/hack.h
lock.o:  ../include/hack.h
mail.o:  ../include/hack.h
makemon.o:  ../include/hack.h
mcastu.o:  ../include/hack.h
mhitm.o:  ../include/hack.h ../include/artifact.h
mhitu.o:  ../include/hack.h ../include/artifact.h ../include/edog.h
mklev.o:  ../include/hack.h
mkmaze.o:  ../include/hack.h
mkobj.o:  ../include/hack.h
mkroom.o:  ../include/hack.h
mon.o:  ../include/hack.h ../include/mfndpos.h ../include/artifact.h
mondata.o:  ../include/hack.h ../include/eshk.h ../include/epri.h
monmove.o:  ../include/hack.h ../include/mfndpos.h ../include/artifact.h
monst.o:  ../include/config.h ../include/permonst.h ../include/monsym.h ../include/eshk.h ../include/vault.h ../include/epri.h
mthrowu.o:  ../include/hack.h
music.o:  ../include/hack.h
o_init.o:  ../include/hack.h
objects.o:  ../include/config.h ../include/obj.h ../include/objclass.h ../include/prop.h
objnam.o:  ../include/hack.h
options.o:  ../include/hack.h
pager.o:  ../include/hack.h
pickup.o:  ../include/hack.h
polyself.o:  ../include/hack.h
potion.o:  ../include/hack.h
pray.o:  ../include/hack.h
pri.o:  ../include/hack.h
priest.o:  ../include/hack.h ../include/mfndpos.h ../include/eshk.h ../include/epri.h
prisym.o:  ../include/hack.h ../include/wseg.h ../include/lev.h
read.o:  ../include/hack.h
restore.o:  ../include/hack.h ../include/lev.h ../include/wseg.h
rip.o:  ../include/hack.h
rnd.o:  ../include/hack.h
rumors.o:  ../include/hack.h
save.o:  ../include/hack.h ../include/lev.h ../include/wseg.h
search.o:  ../include/hack.h ../include/artifact.h
shk.o:  ../include/hack.h ../include/eshk.h
shknam.o:  ../include/hack.h ../include/eshk.h
sit.o:  ../include/hack.h
sounds.o:  ../include/hack.h ../include/edog.h ../include/eshk.h
sp_lev.o:  ../include/hack.h ../include/sp_lev.h
spell.o:  ../include/hack.h
steal.o:  ../include/hack.h
termcap.o:  ../include/hack.h
timeout.o:  ../include/hack.h
topl.o:  ../include/hack.h
topten.o:  ../include/hack.h
track.o:  ../include/hack.h
trap.o:  ../include/hack.h ../include/edog.h ../include/trapname.h
u_init.o:  ../include/hack.h
uhitm.o:  ../include/hack.h ../include/artifact.h
vault.o:  ../include/hack.h ../include/vault.h
version.o:  ../include/hack.h ../include/date.h
weapon.o:  ../include/hack.h
were.o:  ../include/hack.h
wield.o:  ../include/hack.h
wizard.o:  ../include/hack.h
worm.o:  ../include/hack.h ../include/wseg.h
worn.o:  ../include/hack.h
write.o:  ../include/hack.h
zap.o:  ../include/hack.h
../include/config.h:  ../include/tradstdc.h ../include/global.h
			touch ../include/config.h
../include/decl.h:  ../include/spell.h ../include/obj.h ../include/you.h ../include/onames.h ../include/pm.h
			touch ../include/decl.h
../include/global.h:  ../include/coord.h ../include/unixconf.h ../include/pcconf.h ../include/tosconf.h ../include/amiconf.h
			touch ../include/global.h
../include/hack.h:  ../include/config.h ../include/decl.h ../include/monsym.h ../include/mkroom.h ../include/objclass.h ../include/gold.h ../include/trap.h ../include/flag.h ../include/rm.h
			touch ../include/hack.h
../include/permonst.h:  ../include/monattk.h ../include/monflag.h
			touch ../include/permonst.h
../include/you.h:  ../include/attrib.h ../include/monst.h ../include/youprop.h
			touch ../include/you.h
../include/youprop.h:  ../include/prop.h ../include/permonst.h ../include/mondata.h ../include/pm.h
			touch ../include/youprop.h
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
