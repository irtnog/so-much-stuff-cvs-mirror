/*
 * phantglobs.c - globals for Phantasia
 */

#include "include.h"

double	Circle;		/* which circle player is in			*/
double	Shield;		/* force field thrown up in monster battle	*/

bool	Beyond;		/* set if player is beyond point of no return	*/
bool	Marsh;		/* set if player is in dead marshes		*/
bool	Throne;		/* set if player is on throne			*/
bool	Changed;	/* set if important player stats have changed	*/
bool	Wizard;		/* set if player is the 'wizard' of the game	*/
bool	Timeout;	/* set if short timeout waiting for input	*/
bool	Windows;	/* set if we are set up for curses stuff	*/
bool	Luckout;	/* set if we have tried to luck out in fight	*/
bool	Foestrikes;	/* set if foe gets a chance to hit in battleplayer()	*/
bool	Echo;		/* set if echo input to terminal		*/

int	Users;		/* number of users currently playing		*/
int	Whichmonster;	/* which monster we are fighting		*/
int	Lines;		/* line on screen counter for fight routines	*/
#ifdef OK_TO_PLAY
int	Okcount;	/* counter for checking ok_to_play		*/
#endif

jmp_buf Fightenv;	/* used to jump into fight routine		*/
jmp_buf Timeoenv;	/* used for timing out waiting for input	*/

long	Fileloc;	/* location in file of player statistics	*/

char	*Login;		/* pointer to login of player			*/
char	*Enemyname;	/* pointer name of monster/player we are battling*/

struct	player	Player;	/* stats for player				*/
struct	player	Other;	/* stats for another player			*/

struct	monster	Curmonster;/* stats for current monster			*/

struct	energyvoid Enrgyvoid;/* energy void buffer			*/

struct	charstats *Statptr;/* pointer into Stattable[]			*/

/* lookup table for character type dependent statistics */
struct	charstats Stattable[7] =
	{
	/* MAGIC USER */
	/* max brains, max mana, weakness, gold tote, ring duration */
	15.0, 200.0, 18.0, 175.0, 10,
	/* quickness strength     mana         energy       brains       magic lvl */
	30, 6, 0.0,  10, 6, 2.0,  50,51,75.0,  30,16,20.0,  60,26, 6.0,	 5, 5,2.75,

	/* FIGHTER */
	/* max brains, max mana, weakness, gold tote, ring duration */
	10.0, 110.0, 15.0, 220.0, 20,
	/* quickness strength     mana         energy       brains       magic lvl */
	30, 6, 0.0,  40,16, 3.0,  30,21,40.0,  45,26,30.0,  25,21, 3.0,	 3, 4, 1.5,

	/* ELF */
	/* max brains, max mana, weakness, gold tote, ring duration */
	12.0, 150.0, 17.0, 190.0, 13,
	/* quickness strength     mana         energy       brains       magic lvl */
	32, 7, 0.0,  35,11, 2.5,  45,46,65.0,  30,21,25.0,  40,26, 4.0,	 4, 4, 2.0,

	/* DWARF */
	/* max brains, max mana, weakness, gold tote, ring duration */
	7.0, 80.0, 13.0, 255.0,  25,
	/* quickness strength     mana         energy       brains       magic lvl */
	25, 6, 0.0,  50,21, 5.0,  25,21,30.0,  60,41,35.0,  20,21, 2.5,	 2, 4, 1.0,

	/* HALFLING */
	/* max brains, max mana, weakness, gold tote, ring duration */
	11.0, 80.0, 10.0, 125.0, 40,
	/* quickness strength     mana         energy       brains       magic lvl */
	34, 0, 0.0,  20, 6, 2.0,  25,21,30.0,  55,36,30.0,  40,36, 4.5,	 1, 4, 1.0,

	/* EXPERIMENTO */
	/* max brains, max mana, weakness, gold tote, ring duration */
	9.0, 90.0, 16.0, 160.0, 20,
	/* quickness strength     mana         energy       brains       magic lvl */
	27, 0, 0.0,  25, 0, 0.0,  100,0, 0.0,  35, 0, 0.0,  25, 0, 0.0,	 2, 0, 0.0,

	/* SUPER */
	/* max brains, max mana, weakness, gold tote, ring duration */
	15.0, 200.0, 10.0, 225.0, 40,
	/* quickness strength     mana         energy       brains       magic lvl */
	38, 0, 0.0,  65, 0, 5.0,  100,0,75.0,  80, 0,35.0,  85, 0, 6.0,	 9, 0,2.75
	};

/* menu of items for purchase */
struct menuitem	Menu[] =
    {
    "Mana", 1,
    "Shield", 5,
    "Book", 200,
    "Sword", 500,
    "Charm", 1000,
    "Quicksilver", 2500,
    "Blessing", 1000,
    };

FILE	*Playersfp;	/* pointer to open player file			*/
FILE	*Monstfp;	/* pointer to open monster file			*/
FILE	*Messagefp;	/* pointer to open message file			*/
FILE	*Energyvoidfp;	/* pointer to open energy void file		*/

char	Databuf[SZ_DATABUF];	/* a place to read data into		*/

/* edit these path definitions to let files reside elsewhere */
#ifdef __STDC__
#define STR(x)	#x
#define STRING(x)	STR(x)
#define FILENM(x)	STRING(DEST) "/" STRING(x)
#else
#define STRING(x)	"x
#define FILENM(x)	STRING(DEST)/x"
#endif
char	Monstfile[] = FILENM(monsters);	/* monster database		*/
char	Peoplefile[] = FILENM(characs);	/* player database		*/
char	Gameprog[] = FILENM(phantasia);	/* game binary			*/
char	Messfile[] = FILENM(mess);	/* player to player messages	*/
char	Lastdead[] = FILENM(lastdead);	/* data on last player killed	*/
char	Helpfile[] = FILENM(phant.help);	/* manual pages			*/
char	Motdfile[] = FILENM(motd);	/* message from 'wizard'	*/
char	Goldfile[] = FILENM(gold);	/* gold collected in taxes	*/
char	Voidfile[] = FILENM(void);	/* energy void database		*/
char	Scorefile[] = FILENM(scoreboard);	/* hi score database		*/
#ifdef ENEMY
char	Enemyfile[] = FILENM(enemy);	/* restricted account database	*/
#endif

/* some canned strings for messages */
char	Illcmd[] = "Illegal command.\n";
char	Illmove[] = "Too far.\n";
char	Illspell[] = "Illegal spell.\n";
char	Nomana[] = "Not enought mana for that spell.\n";
char	Somebetter[] = "But you already have something better.\n";
char	Nobetter[] = "That's no better than what you already have.\n";
