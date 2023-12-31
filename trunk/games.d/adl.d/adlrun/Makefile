MODEL	= -M2l
OPTIONS	= -I${INC} -O
ALL		= adlrun$(EXE)
include $(HOME)/CONFIG
# The include directory
INC = ../include

# The utilities directory
UTL = ../util

# Places relating to the shar command;  B is the base from which to unpack;
# D is the directory into which to unpack.
# AR = shar -c -v
AR1 = post -c -v -p 4 17
AR2 = post -c -v -p 5 17
B = ..
D = adlrun/

# Where adlrun will reside, after make install
#BIN = /users/cunniff/bin
BIN		= ${GAMLIB}

# Flags for the linker - you may need to change -lcurses to -ltermcap
# or -ltermlib.
#LIBS = -lcurses

# Flags for lint.
LFLAGS = ${DFLAGS} -DLINT -I${INC} -C


# Utilities used by adlrun
UTIL =	${UTL}/virtmem.o ${UTL}/vstring.o ${UTL}/mytime.o

# Headers used in all adlrun source files
HDRS =	${INC}/adltypes.h	${INC}/adlprog.h	${INC}/adldef.h	\
	${INC}/adlrun.h		${INC}/builtins.h	${INC}/virtmem.h \
	${INC}/vstring.h

# The adlrun object files
OBJS =	adlarith.o adldwim.o adlfuncs.o adlintrn.o \
	adlmach.o adlmacro.o adlmiscr.o adlobjr.o \
	adlrun.o adlspec.o adlstr.o adltrans.o \
	rtlex.o rtparse.o adlerr.o adlscrn.o \
	rtdict.o rtglob.o

shar		: adlrun1.shar	adlrun2.shar

adlrun1.shar	: README	COPYRIGHT	Makefile	adlarith.c \
		  adldwim.c	adlerr.c	adlfuncs.c	adlintrn.c \
		  adlmach.c	adlmacro.c	adlmiscr.c	adlobjr.c \
		  adlrun.c
	( cd ${B} ; ${AR1} \
		${D}README	${D}COPYRIGHT	${D}Makefile	${D}adlarith.c \
		${D}adldwim.c	${D}adlerr.c	${D}adlfuncs.c	${D}adlintrn.c \
		${D}adlmach.c	${D}adlmacro.c	${D}adlmiscr.c	${D}adlobjr.c \
		${D}adlrun.c \
	> ${D}adlrun1.shar )

adlrun2.shar	: adlscrn.c	adlspec.c	adlstr.c	adltrans.c \
		  rtdict.c	rtglob.c	rtlex.c		rtparse.c
	( cd ${B} ; ${AR2} \
		${D}adlscrn.c	${D}adlspec.c	${D}adlstr.c	${D}adltrans.c \
		${D}rtdict.c	${D}rtglob.c	${D}rtlex.c	${D}rtparse.c \
	> ${D}adlrun2.shar )

install		: adlrun$(EXE)
	strip adlrun$(EXE)
	cp adlrun$(EXE) ${BIN}
	chmod 755 ${BIN}/adlrun$(EXE)

lint		:
	lint ${LFLAGS} *.c ${UTL}*.c

clean		:
	rm -f ${OBJS} core adlrun1.shar adlrun2.shar adlrun$(EXE)

adlrun$(EXE)	: ${OBJS} ${UTIL}
	$(CC) $(CFLAGS) -o $@ ${OBJS} ${UTIL} ${LIBS}

adlerr.o	: adlerr.c	${HDRS}

adlarith.o	: adlarith.c	${HDRS}

adldwim.o	: adldwim.c	${HDRS}

adlfuncs.o	: adlfuncs.c	${HDRS}

adlintrn.o	: adlintrn.c	${HDRS}

adlmach.o	: adlmach.c	${HDRS}

adlmacro.o	: adlmacro.c	${HDRS}

adlmiscr.o	: adlmiscr.c	${HDRS}

adlobjr.o	: adlobjr.c	${HDRS}

adlrun.o	: adlrun.c	${HDRS}

adlscrn.o	: adlscrn.c	${HDRS}

adlspec.o	: adlspec.c	${HDRS}

adlstr.o	: adlstr.c	${HDRS}

adltrans.o	: adltrans.c	${HDRS}

rtlex.o		: rtlex.c	${HDRS}

rtparse.o	: rtparse.c	${HDRS}

rtdict.o	: rtdict.c	${HDRS}

rtglob.o	: rtglob.c	${HDRS}
