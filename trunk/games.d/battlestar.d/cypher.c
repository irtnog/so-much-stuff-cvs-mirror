#ifndef lint
static char *sccsid = "@(#)cypher.c	1.2 11/28/84";
#endif

#include "externs.h"

cypher()
{
	register int n;
	int junk;
	int lflag = -1;
	char buffer[10];

	while (wordtype[wordnumber] == ADJS)
		wordnumber++;
	while (wordnumber <= wordcount) {
		switch(wordvalue[wordnumber]) {

			case UP:
				if (location[position].access || wiz || tempwiz) {
					if (!location[position].access)
						puts("Zap!  A gust of wind lifts you up.");
					if (!move(location[position].up, AHEAD))
						return(-1);
				} else {
					puts("There is no way up");
					return(-1);
				}
				lflag = 0;
				break;

			 case DOWN:
				if (!move(location[position].down, AHEAD))
					return(-1);
				lflag = 0;
				break;

			 case LEFT:
				if (!move(left, LEFT))
					return(-1);
				lflag = 0;
				break;

			 case RIGHT:
				if (!move(right, RIGHT))
					return(-1);
				lflag = 0;
				break;
			 
			 case AHEAD:
				if (!move(ahead, AHEAD))
					return(-1);
				lflag = 0;
				break;
			 
			 case BACK:
				if (!move(back, BACK))
					return(-1);
				lflag = 0;
				break;
			 
			 case SHOOT:
				if (wordnumber < wordcount && wordvalue[wordnumber+1] == EVERYTHING){
					for (n=0; n < NUMOFOBJECTS; n++)
						if (testbit(location[position].objects,n) && objsht[n]){
							wordvalue[wordnumber+1] = n;
							wordnumber = shoot();
						}
				wordnumber++;
				wordnumber++;
				}
				else 
					shoot();
				break;

			 case TAKE:
				if (wordnumber < wordcount && wordvalue[wordnumber+1] == EVERYTHING){
					for (n=0; n < NUMOFOBJECTS; n++)
						if (testbit(location[position].objects,n) && objsht[n]){
							wordvalue[wordnumber+1] = n;
							wordnumber = take(location[position].objects);
						}
				wordnumber++;
				wordnumber++;
				}
				else 
					take(location[position].objects);
				break;

			 case DROP:

				if (wordnumber < wordcount && wordvalue[wordnumber+1] == EVERYTHING){
					for (n=0; n < NUMOFOBJECTS; n++)
						if (testbit(inven,n)){
							wordvalue[wordnumber+1] = n;
							wordnumber = drop("Dropped");
						}
				wordnumber++;
				wordnumber++;
				}
				else 
					drop("Dropped");
				break;


			 case KICK:
			 case THROW:
				if (wordnumber < wordcount && wordvalue[wordnumber+1] == EVERYTHING){
					for (n=0; n < NUMOFOBJECTS; n++)
						if (testbit(inven,n) ||
						  testbit(location[position].objects, n) && objsht[n]){
							wordvalue[wordnumber+1] = n;
							wordnumber = throw(wordvalue[wordnumber] == KICK ? "Kicked" : "Thrown");
						}
					wordnumber += 2;
				} else 
					throw(wordvalue[wordnumber] == KICK ? "Kicked" : "Thrown");
				break;

			 case TAKEOFF:
				if (wordnumber < wordcount && wordvalue[wordnumber+1] == EVERYTHING){
					for (n=0; n < NUMOFOBJECTS; n++)
						if (testbit(wear,n)){
							wordvalue[wordnumber+1] = n;
							wordnumber = takeoff();
						}
					wordnumber += 2;
				}
				else 
					takeoff();
				break;


			 case DRAW:

				if (wordnumber < wordcount && wordvalue[wordnumber+1] == EVERYTHING){
					for (n=0; n < NUMOFOBJECTS; n++)
						if (testbit(wear,n)){
							wordvalue[wordnumber+1] = n;
							wordnumber = draw();
						}
					wordnumber += 2;
				}
				else 
					draw();
				break;


			 case PUTON:

				if (wordnumber < wordcount && wordvalue[wordnumber+1] == EVERYTHING){
					for (n=0; n < NUMOFOBJECTS; n++)
						if (testbit(location[position].objects,n) && objsht[n]){
							wordvalue[wordnumber+1] = n;
							wordnumber = puton();
						}
					wordnumber += 2;
				}
				else 
					puton();
				break;

			 case WEARIT:

				if (wordnumber < wordcount && wordvalue[wordnumber+1] == EVERYTHING){
					for (n=0; n < NUMOFOBJECTS; n++)
						if (testbit(inven,n)){
							wordvalue[wordnumber+1] = n;
							wordnumber = wearit();
						}
					wordnumber += 2;
				}
				else 
					wearit();
				break;


			 case EAT:

				if (wordnumber < wordcount && wordvalue[wordnumber+1] == EVERYTHING){
					for (n=0; n < NUMOFOBJECTS; n++)
						if (testbit(inven,n)){
							wordvalue[wordnumber+1] = n;
							wordnumber = eat();
						}
					wordnumber += 2;
				}
				else 
					eat();
				break;


			case PUT:
				put();
				break;


			case INVEN:
				if (ucard(inven)){
					puts("You are holding:\n");
					for (n=0; n < NUMOFOBJECTS; n++)
						if (testbit(inven,n))
							printf("\t%s\n", objsht[n]);
					printf("\n= %d kilogram%s (%d%%)\n", carrying, (carrying == 1 ? "." : "s."),(WEIGHT ? carrying*100/WEIGHT : -1));
					printf("Your arms are %d%% full.\n",encumber*100/CUMBER);
				}
				else
					puts("You aren't carrying anything.");
					
				if (ucard(wear)){
					puts("\nYou are wearing:\n");
					for (n=0; n < NUMOFOBJECTS; n++)
						if (testbit(wear,n))
							printf("\t%s\n", objsht[n]);
				}
				else
					puts("\nYou are stark naked.");
				if (card(injuries,NUMOFINJURIES)){
					puts("\nYou have suffered:\n");
					for (n=0; n < NUMOFINJURIES; n++)
						if (injuries[n])
							printf("\t%s\n",ouch[n]);
					printf("\nYou can still carry up to %d kilogram%s\n",WEIGHT,(WEIGHT == 1 ? "." : "s."));
				}
				else
					puts("\nYou are in perfect health.");
				break;

			case USE:
				lflag = use();
				break;

			case LOOK:
				if (!notes[CANTSEE] || testbit(inven,LAMPON) || testbit(location[position].objects,LAMPON) || matchlight){
					beenthere[position] = 2;
					writedes();
					printobjs();
					if (matchlight){
						puts("\nYour match splutters out.");
						matchlight = 0;
					}
				} else 
					puts("I can't see anything.");
				return(-1);
				break;

			 case SU:
			 if (wiz || tempwiz){
				printf("\nRoom (was %d) = ", position);
				fgets(buffer,10,stdin);
				if (*buffer != '\n')
					sscanf(buffer,"%d", &position);
				printf("Time (was %d) = ",gtime);
				fgets(buffer,10,stdin);
				if (*buffer != '\n')
					sscanf(buffer,"%d", &gtime);
				printf("Fuel (was %d) = ",fuel);
				fgets(buffer,10,stdin);
				if (*buffer != '\n')
					sscanf(buffer,"%d", &fuel);
				printf("Torps (was %d) = ",torps);
				fgets(buffer,10,stdin);
				if (*buffer != '\n')
					sscanf(buffer,"%d", &torps);
				printf("CUMBER (was %d) = ",CUMBER);
				fgets(buffer,10,stdin);
				if (*buffer != '\n')
					sscanf(buffer,"%d", &CUMBER);
				printf("WEIGHT (was %d) = ",WEIGHT);
				fgets(buffer,10,stdin);
				if (*buffer != '\n')
					sscanf(buffer,"%d",&WEIGHT);
				printf("Clock (was %d) = ",gclock);
				fgets(buffer,10,stdin);
				if (*buffer != '\n')
					sscanf(buffer,"%d",&gclock);
				printf("Wizard (was %d, %d) = ",wiz, tempwiz);
				fgets(buffer,10,stdin);
				if (*buffer != '\n'){
					sscanf(buffer,"%d",&junk);
					if (!junk)
						tempwiz = wiz = 0;
				}
				printf("\nDONE.\n");
				return(0);
			 }
			 else
				 puts("You aren't a wizard.");
			 break;
				
			 case SCORE:
				printf("\tPLEASURE\tPOWER\t\tEGO\n");
				printf("\t%3d\t\t%3d\t\t%3d\n\n",pleasure,power,ego);
				printf("This gives you the rating of %s in %d turns.\n",rate(),gtime);
				printf("You have visited %d out of %d rooms this run (%d%%).\n",card(beenthere,NUMOFROOMS),NUMOFROOMS,card(beenthere,NUMOFROOMS)*100/NUMOFROOMS);
				break;

			 case KNIFE:
			 case KILL:
				murder();
				break;

			 case UNDRESS:
			 case RAVAGE:
				ravage();
				break;

			 case SAVE:
				save();
				break;

			 case FOLLOW:
				lflag = follow();
				break;

			 case GIVE:
				give();
				break;

			 case KISS:
				kiss();
				break;

			 case LOVE:
				 love();
				 break;

			 case RIDE:
				lflag = ride();
				break;

			 case DRIVE:
				lflag = drive();
				break;

			 case LIGHT:
				 light();
				 break;

			 case LAUNCH:
				if (!launch())
					return(-1);
				else 
					lflag = 0;
				break;

			case LANDIT:
				if (!land())
					return(-1);
				else
					lflag = 0;
				break;

			case TIME:
				chime();
				break;

			 case SLEEP:
				zzz();
				break;

			 case DIG:
				dig();
				break;

			 case JUMP:
				lflag = jump();
				break;

			 case BURY:
				bury();
				break;

			 case SWIM:
				puts("Surf's up!");
				break;

			 case DRINK:
				drink();
				break;

			 case QUIT:
				die();

			 default:
				puts("How's that?");
				return(-1);
				break;

			 
		}
		if (wordnumber < wordcount && *words[wordnumber++] == ',')
			continue;
		else return(lflag);
       }
       return(lflag);
}
