###########################################################################
# This program is Copyright (C) 1986, 1987, 1988 by Jonathan Payne.  JOVE #
# is provided to you without charge, and with no warranty.  You may give  #
# away copies of JOVE, including sources, provided that this notice is    #
# included in all the files.                                              #
###########################################################################

# TMPDIR is where the tmp files get stored, usually /tmp or /tmp/jove.  If
# your system does not remove subdirectories of /tmp on reboot (lots do
# remove them these days) then it makes sense to make TMPDIR be /tmp/jove.
# But if you want to recover buffers on system crashes, you should create a
# directory that doesn't get clearned upon reboot, and use that instead.
# You would probably want to clean out that directory periodically with
# /etc/cron.  LIBDIR is for online documentation, the PORTSRV process,
# RECOVER, and the system-wide .joverc file.  BINDIR is where to put the
# executables JOVE and TEACHJOVE.  MANDIR is where the manual pages go for
# JOVE, RECOVER and TEACHJOVE.  MANEXT is the extension for the man pages,
# e.g., jove.1 or jove.l or jove.m.

#DESTDIR =
#TMPDIR = /tmp
#LIBDIR = /usr/local/lib/jove
#BINDIR = /usr/local
MANDIR = $(MAN1)
MANEXT = 1
#SHELL = /bin/csh
#all:	sdate xjove$(EXE) recover$(EXE) teachjove$(EXE) portsrv$(EXE) macvert$(EXE) edate
# for SCO Xenix, set
#	MEMFLAGS = -Mle
#	CFLAGS = -LARGE -O -F 3000 -K -Mle  (say -Mle2 for an 80286)
#CFLAGS = -O $(CFLAGS)
ALL		= sdate xjove$(EXE) recover$(EXE) teachjove$(EXE) portsrv$(EXE) macvert$(EXE) edate
include $(HOME)/CONFIG

# These should all just be right if the above ones are.
JOVE = $(DESTDIR)$(BINDIR)/jove$(EXE)
TEACHJOVE = $(DESTDIR)$(BINDIR)/teachjove
RECOVER = $(DESTDIR)$(LIBDIR)/recover$(EXE)
PORTSRV = $(DESTDIR)$(LIBDIR)/portsrv$(EXE)
JOVERC = $(DESTDIR)$(LIBDIR)/.joverc
CMDS.DOC = $(DESTDIR)$(LIBDIR)/cmds.doc
TEACH-JOVE = $(DESTDIR)$(LIBDIR)/teach-jove
JOVEM = $(DESTDIR)$(MANDIR)/jove.$(MANEXT)
TEACHJOVEM = $(DESTDIR)$(MANDIR)/teachjove.$(MANEXT)

# Select the right libraries for your system.
#	2.10BSD:LIBS = -ltermcap
#	v7:	LIBS = -ltermcap
#	4.1BSD:	LIBS = -ltermcap -ljobs
#	4.2BSD:	LIBS = -ltermcap
#	4.3BSD:	LIBS = -ltermcap
#	SysV Rel. 2: LIBS = -lcurses
#	SCO Xenix: LIBS = -ltermcap -lx
#LIBS = -ltermcap

# If you are not VMUNIX (vax running Berkeley Version 4), you must specify
# the -i flags (split I/D space) and maybe the -x option (for adb to work).
#	2.10BSD:LDFLAGS =
#	v7:	LDFLAGS =
#	4.1BSD:	LDFLAGS =
#	4.2BSD:	LDFLAGS =
#	4.3BSD:	LDFLAGS =
#	SysV Rel. 2: LDFLAGS = -Ml
#	SCO Xenix: LDFLAGS = -Ml -F 3000
#
# SEPFLAG should be:
#	not on a PDP-11:		SEPFLAG =
#	PDP-11 with separate I&D:	SEPFLAG = -i
#	PDP-11 without separate I&D:	SEPFLAG = -n
#

LDFLAGS = 

SEPFLAG =

BASESEG = funcdefs.o keymaps.o argcount.o ask.o buf.o ctype.o delete.o \
	  disp.o insert.o io.o jove.o malloc.o marks.o misc.o re.o \
	  screen.o table.o tune.o util.o vars.o version.o
OVLAY1 = abbrev.o rec.o paragraph.o fmt.o
OVLAY2 = c.o wind.o fp.o move.o
OVLAY3 = extend.o macros.o
OVLAY4 = iproc.o re1.o
OVLAY5 = proc.o scandir.o term.o case.o

OBJECTS = $(BASESEG) $(OVLAY1) $(OVLAY2) $(OVLAY3) $(OVLAY4) $(OVLAY5)

C_SRC = funcdefs.c abbrev.c argcount.c ask.c buf.c c.c case.c ctype.c \
	delete.c disp.c extend.c fp.c fmt.c insert.c io.c iproc.c \
	jove.c macros.c malloc.c marks.c misc.c move.c paragraph.c \
	proc.c re.c re1.c rec.c scandir.c screen.c table.c term.c util.c \
	vars.c version.c wind.c getch.c mac.c

SOURCES = $(C_SRC) portsrv.c recover.c setmaps.c teachjove.c

HEADERS = ctype.h io.h jove.h re.h rec.h table.h temp.h termcap.h \
	tune.h externs.h mac.h

DOCS =	doc/cmds.doc.nr doc/example.rc doc/jove.1 doc/jove.2 doc/jove.3 \
	doc/jove.4 doc/jove.5 doc/jove.nr doc/system.rc \
	doc/teach-jove doc/teachjove.nr doc/README doc/jove.qref

MISC = Makefile Ovmakefile Makefile.dos tune.dos tune.template \
	README Readme.dos Readme.mac iproc-pipes.c iproc-ptys.c

SUPPORT = teachjove.c recover.c setmaps.c portsrv.c keymaps.txt \
	macvert.c menumaps.txt mjovers.Hqx

BACKUPS = $(HEADERS) $(C_SRC) $(DOCS) $(SUPPORT) $(MISC)

sdate:
	@/bin/echo "**** make started at `date` ****"

edate:
	@echo "**** make completed at `date` ****"

xjove$(EXE):	$(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)
	@-size $@

gjove:	$(OBJECTS)
	ld -X /lib/gcrt0.o -o $@ $(OBJECTS) -lc $(LIBS)
	@-size gjove

ovjove:	$(OBJECTS)
	ld $(SEPFLAG) $(LDFLAGS) -X /lib/crt0.o \
		-Z $(OVLAY1) \
		-Z $(OVLAY2) \
		-Z $(OVLAY3) \
		-Z $(OVLAY4) \
		-Z $(OVLAY5) \
		-Y $(BASESEG) \
		-o $@ $(LIBS) -lc; \
	@-size $@

portsrv$(EXE):	portsrv.o
	$(CC) $(LDFLAGS) -o $@ $(SEPFLAG) portsrv.o $(LIBS)

recover$(EXE):	recover.o tune.o rec.h temp.h
	$(CC) $(LDFLAGS) -o $@ $(SEPFLAG) recover.o tune.o $(LIBS)

teachjove$(EXE):	teachjove.o
	$(CC) $(LDFLAGS) -o $@ $(SEPFLAG) teachjove.o $(LIBS)

setmaps$(EXE):	setmaps.o funcdefs.c
	$(CC) $(LDFLAGS) -o $@ setmaps.o

teachjove.o:	teachjove.c /usr/include/sys/types.h /usr/include/sys/file.h
	$(CC) -c $(CFLAGS) -DTEACHJOVE=\"$(TEACH-JOVE)\" teachjove.c

# don't optimize setmaps.c because it produces bad code in some places
# for some reason
setmaps.o:	funcdefs.c keymaps.txt
	$(CC) $(MEMFLAGS) -c setmaps.c

# ignore error messages from setmaps
# it doesn't understand ifdefs

keymaps.c:	setmaps$(EXE) keymaps.txt
	-./setmaps$(EXE) < keymaps.txt > keymaps.c

keymaps.o:	keymaps.c jove.h

tune.c: Makefile tune.template
	@echo "/* Changes should be made in Makefile, not to this file! */" > tune.c
	@echo "" >> tune.c
	@sed -e 's;TMPDIR;$(TMPDIR);' \
	     -e 's;LIBDIR;$(LIBDIR);' \
	     -e 's;BINDIR;$(BINDIR);' \
	     -e 's;SHELL;$(SHELL);' tune.template >> tune.c

iproc.o: iproc-ptys.c iproc-pipes.c iproc.c
	$(CC) $(CFLAGS) -c iproc.c

macvert$(EXE):	macvert.c
	$(CC) $(CFLAGS) -o $@ macvert.c

install: $(DESTDIR)$(LIBDIR) $(TEACH-JOVE) $(CMDS.DOC) $(JOVERC) \
	 $(PORTSRV) $(RECOVER) $(JOVE) $(TEACHJOVE) $(JOVEM) \
	 $(RECOVERM) $(TEACHJOVEM)

#$(DESTDIR)$(LIBDIR):
#	-mkdir $(DESTDIR)$(LIBDIR)

$(TEACH-JOVE): doc/teach-jove
	cp doc/teach-jove $(TEACH-JOVE)
	chmod 644 $(TEACH-JOVE)

doc/cmds.doc:	doc/cmds.doc.nr doc/jove.4 doc/jove.5
	$(NROFF) doc/cmds.doc.nr doc/jove.4 doc/jove.5 > doc/cmds.doc

$(CMDS.DOC): doc/cmds.doc
	cp doc/cmds.doc $(CMDS.DOC)
	chmod 644 $(CMDS.DOC)

$(JOVERC): doc/system.rc
	cp doc/system.rc $(JOVERC)
	chmod 644 $(JOVERC)

$(PORTSRV): portsrv$(EXE)
	cp portsrv$(EXE) $(PORTSRV)
	chmod 755 $(PORTSRV)

$(RECOVER): recover$(EXE)
	cp recover$(EXE) $(RECOVER)
	chmod 755 $(RECOVER)

$(JOVE): xjove$(EXE)
	cp xjove$(EXE) $(JOVE)
	chmod 755 $(JOVE)

$(TEACHJOVE): teachjove$(EXE)
	cp teachjove$(EXE) $(TEACHJOVE)
	chmod 755 $(TEACHJOVE)

$(JOVEM): doc/jove.nr
	@sed -e 's;TMPDIR;$(TMPDIR);' \
	     -e 's;LIBDIR;$(LIBDIR);' \
	     -e 's;SHELL;$(SHELL);' doc/jove.nr > /tmp/jove.nr
	cp /tmp/jove.nr $(JOVEM)
	chmod 644 $(JOVEM)

$(TEACHJOVEM): doc/teachjove.nr
	@sed -e 's;TMPDIR;$(TMPDIR);' \
	     -e 's;LIBDIR;$(LIBDIR);' \
	     -e 's;SHELL;$(SHELL);' doc/teachjove.nr > /tmp/teachjove.nr
	cp /tmp/teachjove.nr $(TEACHJOVEM)
	chmod 644 $(TEACHJOVEM)

echo:
	@echo $(C-FILES) $(HEADERS)

lint:
	lint -n $(C_SRC) tune.c keymaps.c
	@echo Done

tags:
	ctags -w $(C_SRC) $(HEADERS) iproc-ptys.c

ciall:
	ci $(BACKUPS)

coall:
	co $(BACKUPS)

jove.shar:
	shar $(BACKUPS) > jove.shar

backup: $(BACKUPS)
	tar chf backup $(BACKUPS)

tape-backup:
	tar c $(BACKUPS)

srcdownload:
	kermit -s $(SUPPORT) $(MISC) $(HEADERS) $(C_SRC)

docdownload:
	kermit -s $(DOCS)

touch:
	touch $(OBJECTS)

clean:
	rm -f a.out core *.o keymaps.c tune.c xjove$(EXE) portsrv$(EXE) \
	  recover$(EXE) setmaps$(EXE) teachjove$(EXE) macvert$(EXE)

# This version only works under 4.3BSD
#depend:
#	for i in ${SOURCES} ; do \
#		$(CC) -M ${CFLAGS} $$i | awk ' { if ($$1 != prev) \
#		    { if (rec != "") print rec; rec = $$0; prev = $$1; } \
#		    else { if (length(rec $$2) > 78) { print rec; rec = $$0; } \
#		    else rec = rec " " $$2 } } \
#		    END { print rec } ' >> makedep; done
#	echo '/^# DO NOT DELETE THIS LINE/+2,$$d' >eddep
#	echo '$$r makedep' >>eddep
#	echo 'w' >>eddep
#	cp Makefile Makefile.bak
#	ed - Makefile < eddep
#	rm eddep makedep
#	echo '# DEPENDENCIES MUST END AT END OF FILE' >> Makefile
#	echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> Makefile
#	echo '# see make depend above' >> Makefile
#
## DO NOT DELETE THIS LINE -- make depend uses it
