/*
 *	arcmatch.c	1.1
 *
 *	Author: Thom Henderson
 *	Original System V port: Mike Stump
 *	Enhancements, Bug fixes, and cleanup: Chris Seaman
 *	Date: Fri Mar 20 09:57:02 1987
 *	Last Mod.	3/22/87
 *
 */

/*
 * ARC - Archive utility - ARCMATCH
 * 
 * Version 2.17, created on 12/17/85) at 20:32:18
 * 
 * (C) COPYRIGHT 1985 by System Enhancement Associates; ALL RIGHTS RESERVED
 * 
 *     Description:
 *          This file contains service routines needed to maintain an archive.
 */

#include "arc.h"
#include <sys/types.h>
#ifdef SYS5
#include <dirent.h>
#else
#include <sys/dir.h>
#endif

#define ASTERISK '*'		/* The '*' metacharacter */
#define QUESTION '?'		/* The '?' metacharacter */
#define BACK_SLASH '\\'         /* The '\' metacharacter */
#define LEFT_BRACKET '['	/* The '[' metacharacter */
#define RIGHT_BRACKET ']'	/* The ']' metacharacter */

#define IS_OCTAL(ch) (ch >= '0' && ch <= '7')

typedef INT BOOLEAN;
#define VOID short
#define TRUE 1
#define FALSE 0
#define EOS '\000'

static BOOLEAN do_list();
static char nextch();
static VOID list_parse();

int match(string, pattern)
char *string;
char *pattern;
{
    register int ismatch;

    ismatch = FALSE;
    switch (*pattern)
    {
    case ASTERISK:
        pattern++;
        do
        {
            ismatch = match (string, pattern);
        }
        while (!ismatch && *string++ != EOS);
        break;
    case QUESTION:
        if (*string != EOS)
            ismatch = match (++string, ++pattern);
        break;
    case EOS:
        if (*string == EOS)
            ismatch = TRUE;
        break;
    case LEFT_BRACKET:
        if (*string != EOS)
            ismatch = do_list (string, pattern);
        break;
    case BACK_SLASH:
        pattern++;
    default:
        if (*string == *pattern)
        {
            string++;
            pattern++;
            ismatch = match (string, pattern);
        }
        else
            ismatch = FALSE;
        break;
    }
    return(ismatch);
}

static BOOLEAN do_list (string, pattern)
register char *string;
char *pattern;
{
    register BOOLEAN ismatch;
    register BOOLEAN if_found;
    register BOOLEAN if_not_found;
    auto char lower;
    auto char upper;

    pattern++;
    if (*pattern == '!')
    {
        if_found = FALSE;
        if_not_found = TRUE;
        pattern++;
    }
    else
    {
        if_found = TRUE;
        if_not_found = FALSE;
    }
    ismatch = if_not_found;
    while (*pattern != ']' && *pattern != EOS)
    {
        list_parse(&pattern, &lower, &upper);
        if (*string >= lower && *string <= upper)
        {
            ismatch = if_found;
            while (*pattern != ']' && *pattern != EOS) pattern++;
        }
    }

    if (*pattern++ != ']')
        arcabort("Character class error");
    else
        if (ismatch)
            ismatch = match (++string, pattern);

    return(ismatch);
}

static VOID list_parse (patp, lowp, highp)
char **patp;
char *lowp;
char *highp;
{
    *lowp = nextch (patp);
    if (**patp == '-')
    {
        (*patp)++;
        *highp = nextch(patp);
    }
    else
        *highp = *lowp;
}

static char nextch (patp)
char **patp;
{
    register char ch;
    register char chsum;
    register INT count;

    ch = *(*patp)++;
    if (ch == '\\')
    {
        ch = *(*patp)++;
        if (IS_OCTAL (ch))
        {
            chsum = 0;
            for (count = 0; count < 3 && IS_OCTAL (ch); count++)
            {
                chsum *= 8;
                chsum += ch - '0';
                ch = *(*patp)++;
            }
            (*patp)--;
            ch = chsum;
        }
    }
    return(ch);
}
