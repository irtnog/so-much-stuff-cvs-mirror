# Makefile for Phantasia 3.3.2
#
# To create the game:
#
# 1)  Set up a directory where the game and its support files will live.
#        (It is suggested that the source be kept somewhere else.)
#
# 2)  Set up the variables in Makefile CAREFULLY to reflect your
#        particular situation.
#
# 3)  Check out ok_to_play() at the end of main.c if you want to include
#        code to restrict access at certain times.
#
# 4)  'make install' and sit back and watch.
#
# NOTE:  'setup' MUST be executed before trying to run '${GAME}'.
#	 The install procedure does this for you.
#
#
# To convert from 3.3.1 or 3.3.1+ to 3.3.2:
#
# 1)	'make convert'  (3.3.1+: 'make PLUS=-DPHANTPLUS convert')
#
# 2)	Move 'newcharacs' to 'characs'.
#
RM	= rm
SHAR	= /bin/shar
#
# Tell 'make' to use /bin/sh for all commands.
SHELL	= /bin/sh
#
# GAME is the name for the game itself.
GAME	= phantasia
#
# PLOTDEV is used for plotting the map.  Change as appropriate.
PLOTDEV	= > /dev/tty
#
# Add '-ljobs' to LDFLAGS if 4.1bsd; add -ltermlib if not SVR2.
# Need separate i/d space on small machines.
# May need '-f' on machines without floating point hardware.
LDFLAGS	= -lm -lcurses -ltermlib
#
# GAMDIR is where the top level user program resides.
# GAMLIB is where the program and its support files reside.
GAMDIR	= $(DESTDIR)/usr/games
LIBDIR	= $(GAMDIR)/lib
GAMLIB	= $(LIBDIR)/phant3.3.2
#
# The following are program constants which are implementation dependent.
#
# GAMLIB is passed on.  Note that there is no closing quote here.
# WIZARD is the login of the one who will clean up things.
# UID is the uid of game wizard.
# define OK_TO_PLAY to restrict playing access at certain times.
#	Also see function ok_to_play() in main.c, and tailor to your own needs.
# define ENEMY to include code for checking of a 'hit list' of resricted 
#	accounts.  The list of logins goes in the file 'enemy'.
# SHELL is the default shell for shell escapes.  If SHELL is not defined,
#	shell escapes are not enabled.
# define BSD41 for 4.1bsd
# define BSD42 for 4.2bsd
# define SYS3 for System III, or similar (like Version 7)
# define SYS5 for System V
# If you don't have 'strchr()', put '-Dstrchr=index' below.
# May need '-I/usr/include/sys' for 4.2bsd.
CFLAGS	= -DSYS3 -DDEST=\"${GAMLIB} -DWIZARDA=\"root\" -DWIZARDB=\"vrs\" \
	  -DUIDA=0 -DUIDB=244 -DSHELL=\"/bin/sh\" -DENEMY -DOK_TO_PLAY \
	  -M2m -DSYS3 # -O
CFILES	= main.c fight.c io.c interplayer.c gamesupport.c misc.c phantglobs.c
OFILES	= main.o fight.o io.o interplayer.o gamesupport.o misc.o phantglobs.o
HFILES	= phantdefs.h phantstruct.h macros.h include.h

all:		${GAME} setup phant.help

# Make the game.
${GAME}:	${OFILES}
		${CC} -M2m ${OFILES} ${LDFLAGS} -o $@

${OFILES}:	${HFILES}

# Make the initialization program.
setup:		phantglobs.o setup.o monsters.asc
		${CC} -M2m phantglobs.o setup.o ${LDFLAGS} -o setup

setup.o:	${HFILES}

# Make documentation.
# The flags below on tbl and nroff are to make a line printable version.
phant.help:	phant.nr
		nroff -man phant.nr > phant.help

${GAMDIR} ${LIBDIR} ${GAMLIB}:
		mkdir $@
		chmod 755 $@

# Install game and documentation.  Initialize.
install:	all ${GAMDIR} ${LIBDIR} ${GAMLIB}
		cp phant.help ${GAMLIB}
		cp ${GAME} ${GAMLIB}/phantasia
		cp ${GAME} ${GAMDIR}
		chmod 4711 ${GAMLIB}/phantasia ${GAMDIR}/${GAME}
		chmod 644 ${GAMLIB}/phant.help
		setup

# Make Phantasia map.
# Change the map commands reflect your installation.
# Pre-System III installations may use 'plot' instead of 'tplot'.
map:		map.c
		cc -O map.c -lplot -o map
		map | tplot -t4014 ${PLOTDEV}

# Convert old character file to new format.
# 'convert' is meant to be run only once.
# PLUS = PHANTPLUS for conversion from 3.3.1+.
convert:	convert.c oldplayer.h phantstruct.h
		${CC} ${CFLAGS} ${PLUS} convert.c ${LDFLAGS} -o convert
		convert
		@echo "New data is in newcharacs."

# Clean up directory after installation.
clean:
		${RM} -f *.o ${GAME} phant.help phant.pt? setup

# Make 'shell archives'.
# All source files barely fit into 5 archives, and must be
# carefully grouped to do so.
shar:		${CFILES} ${HFILES} phant.nr Makefile map.c convert.c oldplayer.h
		${SHAR} README Makefile main.c include.h phantstruct.h \
		    map.c monsters.asc > phant.pt1
		${SHAR} phantglobs.h phantglobs.c fight.c \
		    convert.c > phant.pt2
		${SHAR} interplayer.c gamesupport.c io.c > phant.pt3
		${SHAR} macros.h phantdefs.h setup.c misc.c \
		    oldplayer.h > phant.pt4
		${SHAR} phant.nr interplayer.c > phant.pt5
