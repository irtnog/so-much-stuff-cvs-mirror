#ifndef lint
static char *sccsid = "@(#)com2.c	1.2 11/28/84";
#endif

#include "externs.h"

wearit()		/* synonyms = {sheathe, sheath} */
{
	register int n;
	int firstnumber, value;

	firstnumber = wordnumber;
	while(wordtype[++wordnumber] == ADJS);
	while(wordnumber <= wordcount){
		value = wordvalue[wordnumber];
		switch(value){
			
			case -1:
				puts("Wear what?");
				return(firstnumber);

			default:
				printf("You can't wear that!\n");
				return(firstnumber);

			case KNIFE:
		/*	case SHIRT:	*/
			case ROBE:
			case LEVIS:	/* wearable things */
			case SWORD:
			case MAIL:
			case HELM:
			case SHOES:
			case PAJAMAS:
			case COMPASS:
			case LASER:
			case AMULET:
			case TALISMAN:
			case MEDALION:
			case ROPE:
			case RING:
			case BRACELET:
			case GRENADE:

				for (n=0; objsht[value][n]; n++);
				if (testbit(inven,value)){
					clearbit(inven,value);
					setbit(wear,value);
					carrying -= objwt[value];
					encumber -= objcumber[value];
					gtime++;
					printf("You are now wearing %s %s.\n",(objsht[value][n-1] == 's' ? "the" : "a"), objsht[value]);
				}
				else if (testbit(wear,value))
					printf("You are already wearing the %s.\n", objsht[value]);
				else 
					printf("You aren't holding the %s.\n", objsht[value]);
				if (wordnumber < wordcount - 1 && wordvalue[++wordnumber] == AND)
					wordnumber++;
				else 
					return(firstnumber);
		} /* end switch */
	} /* end while */
	puts("Don't be ridiculous.");
	return(firstnumber);
}

put()		/* synonyms = {buckle, strap, tie} */
{
	if (wordvalue[wordnumber + 1] == ON){
		wordvalue[++wordnumber] = PUTON;
		return(cypher());
	}
	if (wordvalue[wordnumber + 1] == DOWN){
		wordvalue[++wordnumber] = DROP;
		return(cypher());
	}
	puts("I don't understand what you want to put.");
	return(-1);

}

draw() 			/* synonyms = {pull, carry} */
{
	return(take(wear));
}

use()
{
	while (wordtype[++wordnumber] == ADJS && wordnumber < wordcount);
	if (wordvalue[wordnumber] == AMULET && testbit(inven,AMULET) && position != FINAL){
		puts("The amulet begins to glow.");
		if (testbit(inven,MEDALION)){
			puts("The medallion comes to life too.");
			if (position == 114){
				location[position].down = 160;
				whichway(location[position]);
				puts("The waves subside and it is possible to descend to the sea cave now.");
				gtime++;
				return(-1);
			}
		}
		puts("A light mist falls over your eyes and the sound of purling water trickles in");
		puts("your ears.   When the mist lifts you are standing beside a cool stream.");
		if (position == 229)
			position = 224;
		else
			position = 229;
		gtime++;
		return(0);
	}
	else if (position == FINAL)
		puts("The amulet won't work in here.");
	else if (wordvalue[wordnumber] == COMPASS && testbit(inven,COMPASS))
		printf("Your compass points %s.\n",truedirec(NORTH,'-'));
	else if (wordvalue[wordnumber] == COMPASS)
		puts("You aren't holding the compass.");
	else if (wordvalue[wordnumber] == AMULET)
		puts("You aren't holding the amulet.");
	else
		puts("There is no apparent use.");
	return(-1);
}

murder()
{
	register int n;

	for (n=0; !((n == SWORD || n == KNIFE || n == TWO_HANDED || n == MACE || n == CLEAVER || n == BROAD || n == CHAIN || n == SHOVEL || n == HALBERD) && testbit(inven,n)) && n < NUMOFOBJECTS; n++);
	if (n == NUMOFOBJECTS)
		puts("You don't have suitable weapons to kill.");
	else {
		printf("Your %s should do the trick.\n",objsht[n]);
		while (wordtype[++wordnumber] == ADJS);
		switch(wordvalue[wordnumber]){
			
			case NORMGOD:
				if (testbit(location[position].objects,BATHGOD)){
					puts("The goddess's head slices off.  Her corpse floats in the water.");
					clearbit(location[position].objects,BATHGOD);
					setbit(location[position].objects,DEADGOD);
					power += 5;
					notes[JINXED]++;
				} else if (testbit(location[position].objects,NORMGOD)){
					puts("The goddess pleads but you strike her mercilessly.  Her broken body lies in a\npool of blood.");
					clearbit(location[position].objects,NORMGOD);
					setbit(location[position].objects,DEADGOD);
					power += 5;
					notes[JINXED]++;
					if (wintime)
						live();
				} else puts("I dont see her anywhere.");
				break;
			case TIMER:
				if (testbit(location[position].objects,TIMER)){
					puts("The old man offers no resistance.");
					clearbit(location[position].objects,TIMER);
					setbit(location[position].objects,DEADTIME);
					power++;
					notes[JINXED]++;
				} else puts("Who?");
				break;
			case NATIVE:
				if (testbit(location[position].objects,NATIVE)){
					puts("The girl screams as you cut her body to shreds.  She is dead.");
					clearbit(location[position].objects,NATIVE);
					setbit(location[position].objects,DEADNATIVE);
					power += 5;
					notes[JINXED]++;
				} else puts("What girl?");
				break;
			case MAN:
				if (testbit(location[position].objects,MAN)){
					puts("You strike him to the ground, and he coughs up blood.");
					puts("Your fantasy is over.");
					die();
				}
			case -1:
				puts("Kill what?");
				break;

			default:
				if (wordtype[wordnumber] != NOUNS)
					puts("Kill what?");
				else
					printf("You can't kill that!\n");
		}
	}
}

ravage()
{
	while (wordtype[++wordnumber] != NOUNS && wordnumber <= wordcount);
	if (wordtype[wordnumber] == NOUNS && testbit(location[position].objects,wordvalue[wordnumber])){
		gtime++;
		switch(wordvalue[wordnumber]){
			case NORMGOD:
				puts("You attack the goddess, and she screams as you beat her.  She falls down");
				puts("crying and tries to hold her torn and bloodied dress around her.");
				power += 5;
				pleasure += 8;
				ego -= 10;
				wordnumber--;
				godready = -30000;
				murder();
				win = -30000;
				break;
			case NATIVE:
				puts("The girl tries to run, but you catch her and throw her down.  Her face is");
				puts("bleeding, and she screams as you tear off her clothes.");
				power += 3;
				pleasure += 5;
				ego -= 10;
				wordnumber--;
				murder();
				if (rnd(100) < 50){
					puts("Her screams have attracted attention.  I think we are surrounded.");
					setbit(location[ahead].objects,WOODSMAN);
					setbit(location[ahead].objects,DEADWOOD);
					setbit(location[ahead].objects,MALLET);
					setbit(location[back].objects,WOODSMAN);
					setbit(location[back].objects,DEADWOOD);
					setbit(location[back].objects,MALLET);
					setbit(location[left].objects,WOODSMAN);
					setbit(location[left].objects,DEADWOOD);
					setbit(location[left].objects,MALLET);
					setbit(location[right].objects,WOODSMAN);
					setbit(location[right].objects,DEADWOOD);
					setbit(location[right].objects,MALLET);
				}
				break;
			default:
				puts("You are perverted.");
		}
	}
	else
		puts("Who?");
}

follow()
{
	if (followfight == gtime){
		puts("The Dark Lord leaps away and runs down secret tunnels and corridoors.");
		puts("You chase him through the darkness and splash in pools of water.");
		puts("You have cornered him.  His laser sword extends as he steps forward.");
		position = FINAL;
		fight(DARK,75);
		setbit(location[position].objects,TALISMAN);
		setbit(location[position].objects,AMULET);
		return(0);
	}
	else if (followgod == gtime){
		puts("The goddess leads you down a steamy tunnel and into a high, wide chamber.");
		puts("She sits down on a throne.");
		position = 268;
		setbit(location[position].objects,NORMGOD);
		notes[CANTSEE] = 1;
		return(0);
	}
	else 
		puts("There is no one to follow.");
	return(-1);
}
