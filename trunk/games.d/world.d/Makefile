# Makefile for world and support programs for Unix
#
# The program has been tested and compiles and runs properly on the
# following systems:
# 
# 1)  VAX/VMS Digital CC compiler. No particular tricks are needed.
# 2)  IBM PC-AT Xenix 1.00. It seems to be necessary to use the large
#     memory model, although I'm not sure why since it runs on the
#     same machine under DOS in the small model.
# 3)  IBM PC under Microsoft C version 4.00. It works in the default
#     small memory model if you compile with the -Os switch. Note
#     that on this setup you MUST link the main program World with
#     binmode.obj, provided by Microsoft, for it to work right.
#     Vtxtcn and vcnvrt must NOT be linked with binmode.obj.
#     This compiler generates a few harmless warning messages.
#     You may need to use EXEMOD to increase the stack size.
# 4)  VAX/4.3bsd Unix. No particular tricks needed.
#
# NOTE: Edit helper.c to adjust the pathname for q1text.dat for
# your particular system.
#

WHEADERS = arrays.h variab.h
WOBJS = demons.o helper.o motion.o parser.o verbs1.o verbs2.o world.o
CFLAGS = -O

all:	world

install: 	world
	cp world /usr/games/world
	cp q1text.dat /usr/games/lib/q1text.dat

convert:	vcnvrt vtxtcn vtext.dat
	@echo "creating data files..."
	./vtxtcn
	./vcnvrt
	touch convert

vtext.dat:
	cat vtext.dat.aa vtext.dat.ab vtext.dat.ac >vtext.dat

vcnvrt:	vcnvrt.c
	cc $(CFLAGS) -o vcnvrt vcnvrt.c

vtxtcn:	vtxtcn.c
	cc $(CFLAGS) -o vtxtcn vtxtcn.c

world:	convert $(WOBJS) $(WHEADERS) 
	cc -s -o world $(WOBJS)