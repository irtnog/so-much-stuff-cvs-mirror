#	Makefile for Cake
#
#	$Header: /home/Vince/cvs/local.d/cake.d/Makefile,v 1.1 1987-11-16 17:44:41 vrs Exp $

HDR   =	cake.h conf.h port.h
SRC   =	act.c chase.c entry.c error.c expand.c file.c \
	list.c main.c make.c mem.c pat.c print.c proc.c \
	sym.c sys.c table.c test.c trail.c
OBJ   =	act.o chase.o entry.o error.o expand.o file.o \
	list.o main.o make.o mem.o pat.o print.o proc.o \
	sym.o sys.o table.o test.o trail.o
GEN   =	cake_g.y cake_s.l
GHDR  =	cake_g.h
GSRC  =	cake_g.c cake_s.c
GOBJ  =	cake_g.o cake_s.o
DEST  = /mip/usr/bin
COPTS = -DCAKEDEBUG -DYYDEBUG -DCAREFUL
CFLAGS= $(COPTS)
LDFLAGS=-k 64k

cake:		$(OBJ) $(GOBJ)
		$(CC) $(LDFLAGS) $(CFLAGS) -o cake $(OBJ) $(GOBJ)
		-/bin/rm fake
		ln cake fake

all:		cake lint tags defs

install:	cake
		mv $(DEST)/cake /tmp/Oldcake
		cp cake $(DEST)/cake
		strip $(DEST)/cake

$(OBJ):		$(HDR)
$(GOBJ):	$(HDR)

cake_g.c:	cake_g.y
		yacc -dv cake_g.y
		@mv y.tab.h cake_g.h
		@mv y.tab.c cake_g.c
		@mv y.output cake_g.t

cake_s.c:	cake_s.l
		lex cake_s.l
		@mv lex.yy.c cake_s.c

var:		$(SRC) $(GSRC)
		var -DATT $(COPTS) $(SRC) $(GSRC) > Var

plint:		$(SRC) $(GSRC)
		lint -acpx $(COPTS) $(SRC) $(GSRC) > Lint_errs

ulint:		$(SRC) $(GSRC)
		lint -v $(COPTS) $(SRC) $(GSRC) > Lint_errs

lint:		$(SRC) $(GSRC)
		lint -v $(COPTS) $(SRC) $(GSRC) > Lint_errs
		@gred yy Lint_errs
		@gred malloc Lint_errs
		@gred strcpy Lint_errs
		@gred strcat Lint_errs
		@gred printf Lint_errs
		@gred flsbuf Lint_errs
		@gred setbuf Lint_errs
		@gred rcs_id Lint_errs
		@gred longjmp Lint_errs
		@gred sbrk Lint_errs

cchk:		$(SRC)
		cchk $(SRC) > Cchk_errs

tags:		$(SRC) $(GSRC)
		ctags $(SRC) $(GSRC)

defs:		$(SRC) $(GSRC)
		defn $(SRC) $(GSRC) > defs

cxref:		$(SRC)
		cxref -C $(SRC) > Cxref

ssize:
		ssize $(SRC) $(GEN)

asize:
		ssize $(SRC) $(GEN) $(GSRC)

clean:		
		/bin/rm	cake_g.h cake_g.t \
			Make_errs Lint_errs Cchk_errs core \
			$(OBJ) $(GOBJ) $(GSRC)

gclean:		
		/bin/rm $(GSRC) $(GHDR)