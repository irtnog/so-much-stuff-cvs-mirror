# Configuration options
#  LARNHOME is the directory where the larn data files will be installed.
#  BINDIR is the directory where the larn binary will be installed.
BINDIR	= $(DESTDIR)/usr/games
LIBDIR	= $(BINDIR)/lib
LARNHOME= $(LIBDIR)/larn12
#
# -ltermlib may need to be changed to -ltermcap on some systems
TERMLIB	= -ltermlib -lx
#
# Available compile time options:
#   WIZID=xxx  - this is the userid (or playerid) of the wizard.  Default is
#		zero (superuser), which disables all wizard functions.
#		Players must have this userid (or playerid) in order to
#		become the non-scoring wizard player.  Definition of WIZID
#		to non-zero will enable the special wizard debugging
#		commands.  For root to become wizard, use WIZID= -1.
#   EXTRA      - incorporates code to gather additional performance statistics
#   TIMECHECK  - incorporates code to disable play during working hours (8-5)
#   SYSV       - use system III/V (instead of V7) type ioctl calls
#   BSD        - use BSD specific features (mostly timer and signal stuff)
#   BSD4.1     - use BSD4.1 to avoid some 4.2 dependencies (must be used with
#		BSD above; do not mix with SYSV)
#   HIDEBYLINK - if defined, the program attempts to hide from ps
#   DOCHECKPOINTS - if not defined, checkpoint files are periodically written
#               by the larn process (no forking) if enabled in the .larnopts
#		description file.  Checkpointing is handy on an unreliable
#		system, but takes CPU. Inclusion of DOCHECKPOINTS will cause
#		fork()ing to perform the checkpoints (again if enabled in
#		the .larnopts file).  This usually avoids pauses in larn
#		while the checkpointing is being done (on large machines).
#   SAVEINHOME - put save files in users HOME instead of LARNHOME the as default
#	VER        - This is the version of the software, example:  12
#	SUBVER     - This is the revision of the software, example:  1
#	FLUSHNO=#  - Set the input queue excess flushing threshold (default 5)
#	NOVARARGS  - Define for systems that don't have varargs (a default
#		varargs will be used).
#	MACRORND   - Define to use macro version of rnd() and rund() (fast&big)
#	UIDSCORE   - Define to use user id's to manage scoreboard.  Leaving
#		this out will cause player id's from the file ".playerids" to
#		be used instead.  (.playerids is created upon demand).  Only
#		one entry per id # is allowed in each scoreboard (winning &
#		non-winning).
#	VT100	  - Compile for using vt100 family of terminals.  Omission of this
#		define will cause larn to use termcap, but it will be MUCH
#		slower due to an extra layer of output interpretation. 
#		Also, only VT100 mode allows 2 different standout modes,
#		inverse video, and bold video.  And only in VT100 mode is
#		the scrolling region of the terminal used (much nicer than
#		insert/delete line sequences to simulate it, if VT100 is
#		omitted).
#	NONAP	  - This causes napms() to return immediately instead of
#		delaying n milliseconds.  This define may be needed on some
#		systems if the nap stuff does not work correctly (possible
#		hang).  The nap() routine is primarilly used to delay for
#		effect when casting missile type spells.
#	KERNELHZ=# - This causes napms() to compute the delay time based on
#		a clock rate of HERNELHZ ticks per second.  The default rate
#		is 60 ticks per second.
#
# OPTIONS = -DEXTRA -DBSD -DVER=12 -DSUBVER=0 -DTIMECHECK -DWIZID=1000 -DVT100 -DHIDEBYLINK
OPTIONS	= -DEXTRA -DSYSV -DVER=12 -DSUBVER=0 -DWIZID=-1 -DKERNELHZ=50
CFLAGS= -M2l -F 2000 $(OPTIONS) -DLARNHOME=\"$(LARNHOME)/\"
# End of configurable make options
########################################################################
#
OBJS= main.o object.o create.o tok.o display.o global.o data.o io.o monster.o\
	store.o diag.o help.o config.o nap.o bill.o scores.o signal.o moreobj.o\
	movem.o regen.o fortune.o savelev.o

DOTFILES= .larn.help .larnmaze .larnopts .lfortune

larn12.0: $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o larn12.0 $(TERMLIB)
	size larn12.0

$(OBJS): header.h

$(BINDDIR) $(LIBDIR) $(LARNHOME):
	mkdir $@
	chmod 755 $@

install: larn12.0 $(BINDDIR) $(LIBDIR) $(LARNHOME)
	chmod a+w $(LARNHOME)
	cp larn12.0 $(BINDIR)/larn
	cp $(DOTFILES) $(LARNHOME)

clean:
	rm -f *.o larn12.0