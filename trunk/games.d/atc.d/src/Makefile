MODEL	= -i -F 2000
#OPTIONS	= -Dsrandom=srand -Drandom=rand
CURSESLIBR= libatc.a
ALL	= $(XLIBR) $(CURSESLIBR)
include $(HOME)/CONFIG
DEST	      = .
EXTHDRS	      = /usr/include/ctype.h \
		/usr/include/curses.h \
		/usr/include/math.h \
		/usr/include/pwd.h \
		/usr/include/signal.h \
		/usr/include/stdio.h \
		/usr/include/string.h \
		/usr/include/sys/file.h \
		/usr/include/sys/ioctl.h \
		/usr/include/time.h
#		/usr/include/sgtty.h
HDRS	      = def.h \
		extern.h \
		include.h \
		struct.h \
		tunable.h
YFLAGS        =	-d
MAKEFILE      = Makefile
OBJS	      = extern.o \
		grammar.o \
		input.o \
		lex.o \
		list.o \
		log.o \
		main.o \
		update.o
CURSES_OBJ    = gcurses.o
LOBJS	      = tunable.o
PRINT	      = pr
SRCS	      = extern.c \
		grammar.y \
		gcurses.c \
		input.c \
		lex.l \
		list.c \
		log.c \
		main.c \
		update.c
LSRCS	      =	tunable.c
MAKEFILES     = Makefile.dist Makefile.parent
ASRCS	      = $(SRCS) $(LSRCS) $(HDRS) $(MAKEFILES)

co:
		co $(ASRCS)

$(CURSESLIBR):	$(OBJS) $(CURSES_OBJ)
		ar r $@ $(OBJS) $(CURSES_OBJ)
		$(RANLIB) $@

clean:;		rm -f $(OBJS) $(CURSES_OBJ) $(ALL) y.tab.h

depend:;	@mkmf -f $(MAKEFILE) LIBR=$(LIBR) DEST=$(DEST)

index:;		@ctags -wx $(HDRS) $(ASRCS)

install:	$(LIBR)
		@echo Installing $(LIBR) in $(DEST)
		@install -s $(LIBR) $(DEST)

print:;		@$(PRINT) $(HDRS) $(ASRCS)

program:        $(LIBR)

tags:           $(HDRS) $(ASRCS); @ctags $(HDRS) $(ASRCS)

update:		$(DEST)/$(LIBR)

$(DEST)/$(LIBR): $(ASRCS) $(HDRS) $(EXTHDRS)
		@make -f $(MAKEFILE) DEST=$(DEST) install

lex.o: grammar.o
###
