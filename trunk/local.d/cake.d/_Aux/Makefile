#	Makefile for some of cake's auxiliary programs
ALL = base ccincl later need needed sub subcmd usrc gsrc
include $(HOME)/CONFIG

all:		$(PROGS)

install:	$(PROGS)
		cp $(PROGS) $(BINDIR)

base:		base.o pattern_s.o
		cc -o base base.o pattern_s.o

ccincl:		ccincl.o ccincl_s.o
		cc -o ccincl ccincl.o ccincl_s.o

later:		later.c
		cc -o later later.c

need:		need.c
		cc -o need need.c

needed:		needed.c
		cc -o needed needed.c

refs:		refs.o pattern_s.o
		cc -o refs refs.o pattern_s.o

sub:		sub.c
		cc -o sub sub.c

subcmd:		subcmd.c
		cc -o subcmd subcmd.c

usrc:		usrc.c
		cc -o usrc usrc.c

gsrc:		gsrc.c
		cc -o gsrc gsrc.c

ccincl_s.c:	ccincl_s.l
		lex ccincl_s.l
		@mv lex.yy.c ccincl_s.c

pattern_s.c:	pattern_s.l
		lex pattern_s.l
		@mv lex.yy.c pattern_s.c

clean:		
		/bin/rm	Make_errs *.o *_s.c

gclean:		
		/bin/rm *_s.c
