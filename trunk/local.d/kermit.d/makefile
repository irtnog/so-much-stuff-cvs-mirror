ALL	= wart$(EXE) kermit$(EXE)
OPTIONS	= -O -DUXIII -DDEBUG -DTLOG
MODEL	= -i
include $(HOME)/CONFIG
kermit$(EXE):	wermit$(EXE)
	rm -f $@
	ln $? $@
install: $(ALL)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
# CKUKER.MAK, Version 2.09, 14 September 87
#
# -- Makefile to build C-Kermit for Unix and Unix-like systems --
#
# Before proceeding, read the instructions below, and also read the file
# ckuker.bwr (the "beware file"), and then rename this file to "makefile"
# or "Makefile", and then:
#
# for Amdahl UTS 2.4 on IBM 370 series & compatible mainframes, "make uts24"
# for Amdahl UTSV IBM 370 series & compatible mainframes, "make sys3"
# for AT&T 3Bx systems except 3b1, "make att3bx"
# for AT&T UNIX PC (7300 and 3b1), "make upc"
# for AT&T generic System III/System V, "make sys3" or "make sys3nid"
# for ATT System V R3, use "make sys5r3".  This is different from the above.
# for BBN C/70 with IOS 2.0, "make c70"
# for Bell Unix Version 7 (aka 7th Edition), "make v7" (but see below)
# for Berkeley Unix 4.x, "make bsd" (tested with 4.1, 4.2, and 4.3)
# for Berkeley Unix 2.9 (DEC PDP-11 or Pro-3xx), "make bsd29"
# for CDC VX/VE 5.2.1 Sys V emulation, "make vxve"
# for CIE Systems 680/20 with Regulus, "make cie"
# for DEC Ultrix 1.1 or 1.2, "make bsd"
# for DEC Pro-350 with Pro/Venix V1.x, "make provx1"
# for DEC Pro-350 with Pro/Venix V2.0 (Sys V), "make sys3nid" 
# for DEC Pro-380 with Pro/Venix V2.0 (Sys V), "make sys3" or "make sys3nid"
# for Fortune 32:16, For:Pro 1.8, "make ft18"
# for HP-9000 Series with HP-UX, "make hpux"
# for IBM 370 Series with IX/370, "make ix370"
# for Interactive System III (PC/IX) on PC/XT, "make pcix"
# for Interactive Sys III on other systems, "make is3"
# for Masscomp variation on Sys III, "make rtu"
# for Microsoft,SCO,IBM Xenix (/286, PC/AT, etc), "make xenix" (but see below)
# for NCR Tower 1632, OS 1.02, "make tower1"
# for NCR Tower 1632 with System V, "make sys3"
# for Sequent Balance 8000, "make bsd"
# for Valid Scaldstar, "make valid"
#
# The result is a runnable program called "wermit" in the current directory.
# After satisfactory testing, you can rename wermit to "kermit" and put it
# where users can find it.
#
# To remove intermediate and object files, "make clean".
# To run lint on the source file, "make lint".
#
##############################################################################
#
# Notes:
#
#  In many cases, the -O (optimize) compiler switch is omitted.  Feel free
#  to add it if you trust your optimizer.  The ckuus2.c module, in particular,
#  tends to make optimizers blow up.
#
#  "make bsd" should produce a working C-Kermit for both 4.1, 4.2, and
#  (hopefully) 4.3bsd on VAX, SUN, Pyramid, and other 4.x systems.
#
#  Either "make sys3" or "make sys3nid" tends to produce a working version on
#  any ATT System III or System V R2 or earlier system, including Motorola Four
#  Phase, Callan, Unistar, Cadmus, NCR Tower, HP9836 Series 200, Plexus,
#  Heurikon, etc etc (for exceptions, see below; AT&T 3Bx systems have their
#  own entry).  As far as C-Kermit goes, there is no functional difference
#  between ATT System III and System V R2, so there is no need for a separate
#  "make sys5" entry (but there is one anyway; it merely invokes "make sys3").
#  But for ATT System V R3, use "make sys5r3".  This is different from the 
#  above because of the redefinition of signal().
#
#  "make sys3nid" is equivalent to "make sys3" but leaves out the -i option,
#  which is used indicate that separate instruction and data (text) spaces are
#  to be used, as on a PDP-11.  Some systems don't support this option, others
#  may require it.  If one of these options doesn't work on your System III
#  or System V system, try the other.
#
#  For Xenix...  What's Xenix?  There are so many different products & versions
#  sold under this name, the name "xenix" is almost meaningless.  IBM, SCO,
#  Microsoft, etc, IBM Xenix 1.0 =(?) Microsoft Xenix 3.0 = ???  Nevertheless,
#  try "make xenix".  If it doesn't work, try the following modifications to
#  this Makefile:
#    Change "CC= cc" to "CC = cc -LARGE"
#    In the "xenix:" make entry, add "-M2m -UM_I86" to the compiler switches
#    (for the IBM PC family), and "-Mm -lx" to the link switches.
#
#  For Unix Version 7, several variables must be defined to the values
#  associated with your system.  BOOTNAME=/edition7 is the kernel image on
#  okstate's Perkin-Elmer 3230.  Others will probably be /unix.  PROCNAME=proc
#  is the name of the structure assigned to each process on okstate's system.
#  This may be "_proc" or some other variation.  See <sys/proc.h> for more info
#  on your systems name conventions.  NPROCNAME=nproc is the name of a
#  Kernal variable that tells how many "proc" structures there are.  Again
#  this may be different on your system, but nproc will probably be somewhere.
#  The variable NPTYPE is the type of the nproc variable -- int, short, etc.
#  which can probably be gleaned from <sys/param.h>.
#  The definition of DIRECT is a little more complicated.  If nlist() returns,
#  for "proc" only, the address of the array, then you should define DIRECT
#  as it is below.  If however, nlist() returns the address of a pointer to
#  the array, then you should give DIRECT a null definition (DIRECT= ).  The
#  extern declaration in <sys/proc.h> should clarify this for you.  If it
#  is "extern struct proc *proc", then you should NOT define DIRECT.  If it
#  is "extern struct proc proc[]", then you should probably define DIRECT as
#  it is below.  See ckuv7.hlp for further information.
#
#  For 2.9bsd, the makefile may use pcc rather than cc for compiles;
#  that's what the CC and CC2 definitions are for (the current version
#  of the makefile uses cc for both; this was tested and seems to work on
#  the DEC Pro 380).  2.9 support basically follows the 4.1 path.
#  Some 2.9 systems use "dir.h" for the directory header file, others
#  will need to change this to "ndir.h".
#
#  The v7 and 2.9bsd versions assume I&D space on a PDP-11.  When building
#  C-Kermit for v7 on a PDP-11, you should probably add the -i option to
#  the link flags.  Without I&D space, overlays would probably have to be
#  used (or code mapping a`la Pro/Venix if that's available).
#
#  Other systems require some special treatment:
#
#  For Ridge32 (ROS3.2), use "make sys3", but
#  1. Use "CFLAGS = -DUXIII -i -O" "LNKFLAGS = -i"
#  2. Don't #include <sys/file.h> in cku[tf]io.c.
#
#  For Whitechapel MG-1 Genix 1.3, use "make bsd", but
#  1. In ckufio.c, have zkself() return 0 or call getpid, rather than getppid,
#     or simply do "return(kill(0,9))" rather than "return(kill getppid(),1)".
#  2. Wart reportedly can't process ckcpro.w; just work directly from ckcpro.c.
#
#  For Pixel 1000, use "make bsd", do step 1 for Whitechapel MG-1, above.
#
#  For Altos 986 with Xenix 3.0, use "make sys3", but
#  1. Get rid of any "(void)"'s (they're only there for Lint anyway)
#  2. In ckcdeb.h, define CHAR to be "char" rather than "unsigned char".
#
#  For IBM PC/AT and clones running Microport Sys 5.2 Rel.2, change
#  'wait(0)' everywhere to 'wait((int *)0)', and use the -Ml link switch.
#
#  Other systems that are close to, but not quite, like Sys III or V, or
#  4.x BSD or V7 -- look at some of the tricks used below and see if you
#  can find a combination that works for you.
#
##############################################################################
#
#  V7-specific variables.
#  These are set up for Perkin-Elmer 3230 V7 Unix:
# 
PROC=proc
DIRECT=
NPROC=nproc
NPTYPE=int
BOOTFILE=/edition7
#
# ( For TRS-80 Xenix, use PROC=_proc, DIRECT=-DDIRECT, NPROC=_Nproc, 
#   NPTYPE=short, BOOTFILE=/xenix )
#
###########################################################################
#
#  Compile and Link variables:
#
LNKFLAGS=
#CC= cc
CC2= $(CC)
#
###########################################################################
#
# Dependencies Section:
#
make: 
	@echo 'Make what?  You must tell which system to make C-Kermit for.'

wermit$(EXE): ckcmai.o ckucmd.o ckuusr.o ckuus2.o ckuus3.o ckcpro.o ckcfns.o \
		 ckcfn2.o ckucon.o ckutio.o ckufio.o ckudia.o ckuscr.o
	$(CC2) $(CFLAGS) -o $@ ckcmai.o ckutio.o ckufio.o ckcfns.o \
		 ckcfn2.o ckcpro.o ckucmd.o ckuus2.o ckuus3.o ckuusr.o \
		 ckucon.o ckudia.o ckuscr.o $(LIBS)

ckcmai.o: ckcmai.c ckcker.h ckcdeb.h

ckuusr.o: ckuusr.c ckucmd.h ckcker.h ckuusr.h ckcdeb.h

ckuus2.o: ckuus2.c ckucmd.h ckcker.h ckuusr.h ckcdeb.h

ckuus3.o: ckuus3.c ckucmd.h ckcker.h ckuusr.h ckcdeb.h

ckucmd.o: ckucmd.c ckucmd.h ckcdeb.h

ckcpro.o: ckcpro.c ckcker.h ckcdeb.h

ckcpro.c: ckcpro.w wart
	./wart ckcpro.w ckcpro.c

ckcfns.o: ckcfns.c ckcker.h ckcdeb.h

ckcfn2.o: ckcfn2.c ckcker.h ckcdeb.h

ckufio.o: ckufio.c ckcker.h ckcdeb.h

ckutio.o: ckutio.c ckcker.h ckcdeb.h

ckucon.o: ckucon.c ckcker.h ckcdeb.h

wart$(EXE): ckwart.o
	$(CC) $(CFLAGS) -o wart ckwart.o $(LIBS)

ckwart.o: ckwart.c

ckudia.o: ckudia.c ckcker.h ckcdeb.h

ckuscr.o: ckuscr.c ckcker.h ckcdeb.h
#
###########################################################################
#
# Make commands for specific systems:
#
#
#Berkeley Unix 4.1 or 4.2 (and presumably also 4.3), also Ultrix-32 1.x, 2.0
bsd:
	make wermit$(EXE) "CFLAGS= -DBSD4 -DDEBUG -DTLOG"


#Berkeley Unix 2.8, 2.9 for PDP-11s with I&D space, maybe also Ultrix-11???
#If you have trouble with this, try removing "-l ndir".  If you still have
#trouble, remove "-DDEBUG -DTLOG".  Or try defining CC and/or CC2 as "pcc"
#instead of "cc".
bsd29:
	make wermit$(EXE) "CFLAGS= -DBSD29 -DDEBUG -DTLOG" \
		"LNKFLAGS= -i -lndir" "CC= cc " "CC2= cc"

#Version 7 Unix
v7:
	make wermit$(EXE) "CFLAGS=-DV7 -DDEBUG -DTLOG -DPROCNAME=\\\"$(PROC)\\\" \
	-DBOOTNAME=\\\"$(BOOTFILE)\\\" -DNPROCNAME=\\\"$(NPROC)\\\" \
	-DNPTYPE=$(NPTYPE) $(DIRECT)"

#System V R3, some things changed since Sys V R2...
sys5r3:
	make wermit$(EXE) "CFLAGS = -DSVR3 -DUXIII -DDEBUG -DTLOG -i -O" \
		"LNKFLAGS = -i"

#In case they type "make sys5"...
sys5:
	make sys3


#Generic ATT System III or System V (with I&D space)
sys3:
	make wermit$(EXE) "CFLAGS = -DUXIII -DDEBUG -DTLOG -i -O" "LNKFLAGS = -i"

#ATT UNIX PC 7300 and 3b1 (renames things for shared library)
upc:
	make wermit$(EXE) \
	"CFLAGS = -DDEBUG -DTLOG -DUNIXPC -Ddial=Dial -Dopeni=Openi -O" \
	"LNKFLAGS ="


#Generic ATT System III or System V (no I&D space)
sys3nid:
	make wermit$(EXE) "CFLAGS = -DUXIII -DDEBUG -DTLOG -O" "LNKFLAGS ="

#AT&T 3B-series computers running System V (other than 3b1, which uses 'upc')
#  Only difference from sys3 is lock file stuff...
att3bx:
	make wermit$(EXE) "CFLAGS = -DUXIII -DATT3BX -DDEBUG -DTLOG -i -O" \
		"LNKFLAGS = -i"

#HP 9000 series 300, 500, 800.
hpux:
	make wermit$(EXE) "CFLAGS = -DUXIII -DHPUX -DDEBUG -DTLOG -O" "LNKFLAGS ="

#Regulus on CIE Systems 680/20
cie:
	make wermit$(EXE) "CFLAGS = -DUXIII -DCIE -DDEBUG -DTLOG -O" "LNKFLAGS ="

#Microsoft "Xenix/286" e.g. for IBM PC/AT
xenix:
	make wermit$(EXE) "CFLAGS= -DXENIX -DUXIII -DDEBUG -DTLOG -F 3000 -i" \
		"LNKFLAGS = -F 3000 -i"


#PC/IX, Interactive Corp System III for IBM PC/XT
pcix:
	make wermit$(EXE) \
	"CFLAGS= -DPCIX -DUXIII -DISIII -DDEBUG -DTLOG -Dsdata=sdatax -O -i" \
		"LNKFLAGS = -i"


#Interactive Corp System III port in general --
is3:
	make wermit$(EXE) \
		"CFLAGS = -DISIII -DUXIII -DDEBUG -DTLOG -Ddata=datax -O -i" \
		"LNKFLAGS = -i"


#Masscomp System III
rtu:
	make wermit$(EXE) "CFLAGS= -UFIONREAD -DUXIII -DDEBUG -DTLOG -O" \
		"LNKFLAGS ="

#DEC Pro-3xx with Pro/Venix V1.0 or V1.1
# Requires code-mapping on non-I&D-space 11/23 processor, plus some
# fiddling to get interrupt targets into resident code section.
provx1:
	make wart "CFLAGS= -DPROVX1" "LNKFLAGS= "
	make wermit$(EXE) "CFLAGS = -DPROVX1 -DDEBUG -DTLOG -md780" \
		"LNKFLAGS= -u _sleep -lc -md780"

#NCR Tower 1632, OS 1.02
tower1:
	make wermit$(EXE) "CFLAGS= -DDEBUG -DTLOG -DTOWER1"

#Fortune 32:16, For:Pro 1.8 (mostly like 4.1bsd)
ft18:
	make wermit$(EXE) "CFLAGS= -DTLOG -DBSD4 -DFT18"

#Valid Scaldstar
#Berkeleyish, but need to change some variable names.
valid:
	make wermit$(EXE) "CFLAGS= -DBSD4 -Dcc=ccx -DFREAD=1"

#IBM IX/370 on IBM 370 Series mainframes
#Mostly like sys3, but should buffer packets.
ix370:
	make wermit$(EXE) "CFLAGS = -DIX370 -DUXIII -DDEBUG -DTLOG -i -O" \
	"LNKFLAGS = -i"

#Amdahl UTS 2.4 on IBM 370 series compatible mainframes.
#Mostly like V7, but can't do initrawq() buffer peeking.
uts24:
	make wermit$(EXE) "CFLAGS=-DV7 -DDEBUG -DTLOG -DPROCNAME=\\\"$(PROC)\\\" \
	-DUTS24 -DBOOTNAME=\\\"$(BOOTFILE)\\\" -DNPROCNAME=\\\"$(NPROC)\\\" \
	-DNPTYPE=$(NPTYPE) $(DIRECT)"

#BBN C/70 with IOS 2.0
#Mostly Berkeley-like, but with some ATTisms
c70:
	make wermit$(EXE) "CFLAGS= -DBSD4 -DC70 -DDEBUG -DTLOG"

#Zilog ZEUS 3.21
zilog:
	make wermit$(EXE) "CFLAGS = -DUXIII -DZILOG -DTLOG -i -O" "LNKFLAGS = -i"

#CDC VX/VE 5.2.1
vxve:
	make wermit$(EXE) "CFLAGS = -DUXIII -DVXVE -i -O" "LNKFLAGS = -i"

#Clean up intermediate and object files
clean:
	rm -f ckcmai.o ckucmd.o ckuusr.o ckuus2.o ckuus3.o ckcpro.o \
	ckcfns.o ckcfn2.o ckucon.o ckutio.o ckufio.o ckudia.o ckuscr.o \
	ckwart.o ckcpro.c wermit$(EXE) $(ALL)

#Run Lint on this mess for the BSD version.
lint:
	-lint -x -DBSD4 -DDEBUG -DTLOG ck[cu]*.[hc] > ck.lint.bsd4

