C
C COPYRIGHT 1980, INFOCOM COMPUTERS AND COMMUNICATIONS, CAMBRIDGE MA. 02142
C ALL RIGHTS RESERVED, COMMERCIAL USAGE STRICTLY PROHIBITED
C WRITTEN BY R. M. SUPNIK
C
#ifndef PDP	/* replaced by C function for pdp */
C GTTIME-- GET TOTAL TIME PLAYED
C
C DECLARATIONS
C
 	SUBROUTINE GTTIME(T)
 	IMPLICIT INTEGER(A-Z)
C
 	COMMON /TIME/ PLTIME,SHOUR,SMIN,SSEC
C
#ifdef SYSV
	INTEGER TM(3)
 	CALL ITIME(TM)
 	H = TM(1)
 	M = TM(2)
 	S = TM(3)
#else
 	CALL ITIME(H,M,S)
#endif
 	T=((H*60)+M)-((SHOUR*60)+SMIN)
 	IF(T.LT.0) T=T+1440
 	T=T+PLTIME
 	RETURN
 	END
#endif PDP
C OPNCLS-- PROCESS OPEN/CLOSE FOR DOORS
C
C DECLARATIONS
C
	LOGICAL FUNCTION OPNCLS(OBJ,SO,SC)
	IMPLICIT INTEGER (A-Z)
	LOGICAL QOPEN
#include "parser.h"
#include "objects.h"
#include "oflags.h"
#include "verbs.h"
C
C FUNCTIONS AND DATA
C
	QOPEN(O)=and(OFLAG2(O),OPENBT).NE.0
C
	OPNCLS=.TRUE.
C						!ASSUME WINS.
	IF(PRSA.EQ.CLOSEW) GO TO 100
C						!CLOSE?
	IF(PRSA.EQ.OPENW) GO TO 50
C						!OPEN?
	OPNCLS=.FALSE.
C						!LOSE
	RETURN
C
50	IF(QOPEN(OBJ)) GO TO 200
C						!OPEN... IS IT?
	CALL RSPEAK(SO)
	OFLAG2(OBJ)=or(OFLAG2(OBJ),OPENBT)
	RETURN
C
100	IF(.NOT.QOPEN(OBJ)) GO TO 200
C						!CLOSE... IS IT?
	CALL RSPEAK(SC)
	OFLAG2(OBJ)=and(OFLAG2(OBJ),not(OPENBT))
	RETURN
C
200	CALL RSPEAK(125+RND(3))
C						!DUMMY.
	RETURN
	END
C LIT-- IS ROOM LIT?
C
C DECLARATIONS
C
	LOGICAL FUNCTION LIT(RM)
	IMPLICIT INTEGER (A-Z)
	LOGICAL QHERE
#include "rooms.h"
#include "rflag.h"
#include "objects.h"
#include "oflags.h"
#include "advers.h"
C
	LIT=.TRUE.
C						!ASSUME WINS
	IF(and(RFLAG(RM),RLIGHT).NE.0) RETURN
C
	DO 1000 I=1,OLNT
C						!LOOK FOR LIT OBJ
	  IF(QHERE(I,RM)) GO TO 100
C						!IN ROOM?
	  OA=OADV(I)
C						!NO
	  IF(OA.LE.0) GO TO 1000
C						!ON ADV?
	  IF(AROOM(OA).NE.RM) GO TO 1000
C						!ADV IN ROOM?
C
C OBJ IN ROOM OR ON ADV IN ROOM
C
100	  IF(and(OFLAG1(I),ONBT).NE.0) RETURN
	  IF((and(OFLAG1(I),VISIBT).EQ.0).OR.
&		((and(OFLAG1(I),TRANBT).EQ.0).AND.
&		(and(OFLAG2(I),OPENBT).EQ.0))) GO TO 1000
C
C OBJ IS VISIBLE AND OPEN OR TRANSPARENT
C
	  DO 500 J=1,OLNT
	    IF((OCAN(J).EQ.I).AND.(and(OFLAG1(J),ONBT).NE.0))
&		RETURN
500	  CONTINUE
1000	CONTINUE
	LIT=.FALSE.
	RETURN
	END
C WEIGHT- RETURNS SUM OF WEIGHT OF QUALIFYING OBJECTS
C
C DECLARATIONS
C
	INTEGER FUNCTION WEIGHT(RM,CN,AD)
	IMPLICIT INTEGER (A-Z)
	LOGICAL QHERE
#include "objects.h"
C
	WEIGHT=0
	DO 100 I=1,OLNT
C						!OMIT BIG FIXED ITEMS.
	  IF(OSIZE(I).GE.10000) GO TO 100
C						!IF FIXED, FORGET IT.
	  IF((QHERE(I,RM).AND.(RM.NE.0)).OR.
&		((OADV(I).EQ.AD).AND.(AD.NE.0))) GO TO 50
	  J=I
C						!SEE IF CONTAINED.
25	  J=OCAN(J)
C						!GET NEXT LEVEL UP.
	  IF(J.EQ.0) GO TO 100
C						!END OF LIST?
	  IF(J.NE.CN) GO TO 25
50	  WEIGHT=WEIGHT+OSIZE(I)
100	CONTINUE
	RETURN
	END
