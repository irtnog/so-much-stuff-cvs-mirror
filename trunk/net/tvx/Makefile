## Makefile for tvx
DEST	      = .

EXTHDRS	      = /usr/include/ctype.h \
		/usr/include/sgtty.h \
		/usr/include/stdio.h \
		/usr/include/sys/ioctl.h \
		/usr/include/sys/types.h \
		/usr/include/sys/stat.h \
		/usr/include/sys/ttychars.h \
		/usr/include/sys/ttydev.h

HDRS	      = tvx_glbl.ic\
		tvx_defs.ic

LDFLAGS	      =

CFLAGS        = -O

LIBS	      = /usr/lib/libtermcap.a

LINKER	      = cc

MAKEFILE      = Makefile

OBJS	      = tvx_unix.o \
		tvx_io.o \
		tvx_lex.o \
		tvx_1.o \
		tvx_2.o \
		tvx_lib.o

PRINT	      = pr

PROGRAM	      = tvx

SRCS	      = tvx_unix.c \
		tvx_io.c \
		tvx_lex.c \
		tvx_1.c \
		tvx_2.c \
		tvx_lib.c

all:		$(PROGRAM)

$(PROGRAM):     $(OBJS) $(LIBS)
		@echo -n "Loading $(PROGRAM) ... "
		@$(LINKER) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)
		@echo "done"

clean:;		@rm -f $(OBJS)

depend:;	@mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)

index:;		@ctags -wx $(HDRS) $(SRCS)

install:	$(PROGRAM)
		@echo Installing $(PROGRAM) in $(DEST)
		@install -s $(PROGRAM) $(DEST)

print:;		@$(PRINT) $(HDRS) $(SRCS)

program:        $(PROGRAM)

tags:           $(HDRS) $(SRCS); @ctags $(HDRS) $(SRCS)

update:		$(DEST)/$(PROGRAM)

$(DEST)/$(PROGRAM): $(SRCS) $(LIBS) $(HDRS) $(EXTHDRS)
		@make -f $(MAKEFILE) DEST=$(DEST) install
###
tvx_unix.o: /usr/include/ctype.h /usr/include/sys/ioctl.h \
	/usr/include/sys/types.h /usr/include/sys/stat.h \
	/usr/include/sys/ttychars.h /usr/include/sys/ttydev.h \
	/usr/include/sgtty.h /usr/include/stdio.h tvx_defs.ic tvx_glbl.ic
tvx_io.o: /usr/include/stdio.h tvx_defs.ic tvx_glbl.ic
tvx_lex.o: /usr/include/stdio.h tvx_defs.ic tvx_glbl.ic
tvx_1.o: /usr/include/stdio.h tvx_defs.ic tvx_glbl.ic
tvx_2.o: /usr/include/stdio.h tvx_defs.ic tvx_glbl.ic
tvxlib.o: /usr/include/stdio.h tvx_defs.ic
