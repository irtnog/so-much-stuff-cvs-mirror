/*
 * header.h - Article header format
 */

/*	@(#)header.h	2.14	8/7/84	*/

#define NUNREC 50

/* article header */
struct	hbuf {
	char	from[BUFLEN];		/* From:		*/
	char	path[PATHLEN];		/* Path:		*/
	char	nbuf[BUFLEN];		/* Newsgroups:		*/
	char	title[BUFLEN];		/* Subject:		*/
	char	oident[BUFLEN];		/* Article-I.D.:	*/
	char	ident[BUFLEN];		/* Message-ID:		*/
	char	replyto[BUFLEN];	/* Reply-To:		*/
	char	followid[BUFLEN];	/* References:		*/
	char	subdate[DATELEN];	/* Date: (submission)	*/
	time_t	subtime;		/* subdate in secs	*/
	char	recdate[DATELEN];	/* Date-Received:	*/
	time_t	rectime;		/* recdate in secs	*/
	char	expdate[DATELEN];	/* Expires:		*/
	time_t	exptime;		/* expdate in secs	*/
	char	ctlmsg[PATHLEN];	/* Control:		*/
	char	sender[BUFLEN];		/* Sender:		*/
	char	followto[BUFLEN];	/* Followup-to:		*/
	char	postversion[BUFLEN];	/* Post-Version:	*/
	char	relayversion[BUFLEN];	/* Relay-Version:	*/
	char	distribution[BUFLEN];	/* Distribution:	*/
	char	organization[BUFLEN];	/* Organization:	*/
	char	numlines[8];		/* Lines:		*/
	int	intnumlines;		/* Integer version	*/
	char	keywords[BUFLEN];	/* Keywords:		*/
	char	approved[BUFLEN];	/* Approved:		*/
	char	nf_id[BUFLEN];		/* Nf-ID:		*/
	char	nf_from[BUFLEN];	/* Nf-From:		*/
	char	*unrec[NUNREC];		/* unrecognized lines	*/
};
