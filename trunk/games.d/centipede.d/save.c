#define BSD
#include "cent.h"

#define MAGICNUM 15
#define GOOD 1
#define BAD 0

extern struct passwd *getpwnam();

savegame()
{
    char fil[100];
#ifdef __STDC__
    struct termios curseterm;
#else
    struct sgttyb curseterm;
#endif

    strcpy(fil, "cent.save");
    if (!gamestarted)
    {
	mvaddstr(15,60,"Too early to save");
	refresh();
	return;
    }
#ifdef SYS5
    clear();
    refresh();
#else
    printf("%s",CL);
#endif
    setblock(0, TRUE);
    nocrmode();
    echo();
    nl();
    printf("File name: ");
#ifdef __STDC__
    tcgetattr(0,&curseterm);
    tcsetattr(0,TCSADRAIN,&origterm);
#else
    ioctl(0,TIOCGETP,&curseterm);
    ioctl(0,TIOCSETP,&origterm);
#endif
    scanf("%s",fil);
#ifdef __STDC__
    tcsetattr(0,TCSADRAIN,&curseterm);
#else
    ioctl(0,TIOCSETP,&curseterm);
#endif
    noecho();
    crmode();
    printf("Saving... ");
    fflush(stdout);
    if (dosave(fil) == GOOD)
    {
	printf("Done.\n");
	endwin();
	exit(0);
    }
    nonl();
    printf("[Press return to continue game]");
    getchar();
    while (getchar() != '\r');
    redrawscr();
}

dosave(fil)
char *fil;
{
    long tim;
    int fd,y,x;
    char ch;
    PEDE *piece;
#ifdef __STDC__
    FILE *fp;
#endif
    
    if ((fd = creat(fil,0600)) == -1)
    {
	perror("Error opening file");
	return(BAD);
    }
#ifdef __STDC__
    if ((fp = fdopen(fd,"wb")) == NULL)
    {
	perror("Error opening file");
	close(fd);
	return(BAD);
    }
#endif
    ch = MAGICNUM;
#ifdef __STDC__

    fwrite(&ch,1,1,fp);
    fwrite(&version,sizeof(float),1,fp);
    putwin(stdscr,fp);
    fwrite(mushw,24 * 57,1,fp);
#define wint(i)  fwrite(&i,sizeof(int),1,fp)
#define wlong(i) fwrite(&i,sizeof(long),1,fp)
#define wc(cd)   fwrite(&cd,sizeof(COORD),1,fp)

#else

    write(fd,&ch,1);
    write(fd,&version,sizeof(float));
    for (y = 0; y < 24; y++)
	write(fd,(stdscr->_y)[y],80);
    write(fd,mushw,24 * 57);
#define wint(i) write(fd,&i,sizeof(int))
#define wlong(i) write(fd,&i,sizeof(long))
#define wc(cd) write(fd,&cd,sizeof(COORD))

#endif

    wint(numpedes);
    wint(fired);
    wlong(score);
    wint(board);
    wint(extramen);
    wlong(nextman);
    wint(dead);
    wint(finished);
    wint(breeding);
    wint(breedtime);
    wint(moves);
    wint(fleahere);
    wint(fleashot);
    wint(nummushrooms);
    wint(scorphere);
    wint(scorpthisboard);
    wint(scorpvel);
    wint(spiderhere);
    wint(spiderdir);
    wint(spidcount);

    wc(guy);
    wc(shot);
    wc(flea);
    wc(scorp);
    wc(spider);
    wc(spidervel);
    write(fd,name,10);
    for (piece = centipede; piece != NULL; piece = piece->next)
	write(fd,piece,sizeof(PEDE));
    tim = time(0);
    wlong(tim);
#ifdef __STDC__
    fclose(fp);
#endif
    close(fd);
    chmod(fil,0);
    return(GOOD);
}

#define BADF(m) {perror(m); chmod(fil,sbuf.st_mode & 07777); exit(0);}

dorest(fil)
char *fil;
{
    int fd,n,count,tim;
    char ch;			/* KDW: was register; blasted for portability */
    register int y,x;
    char buf[512];
    PEDE **piece = &centipede, *prev = NULL;
    struct stat sbuf;
    float vers;
#ifdef __STDC__
    FILE *fp;
#endif

    printf("Reading file... ");
    fflush(stdout);
    stat(fil,&sbuf);
    chmod(fil,0400);
    if ((fd = open(fil,0)) == -1)
	BADF("Error opening file");
    if (getpwnam(getlogin())->pw_uid != sbuf.st_uid)
	BADF("That's not your game!\n");
    if (sbuf.st_nlink > 1)
	BADF("Cannot restore from linked file.\n");
#ifdef __STDC__

    if ((fp = fdopen(fd,"rb")) == NULL)
	BADF("Error fdopening file");
#define rint(i) fread(&i,sizeof(int),1,fp);
#define rlong(i) fread(&i,sizeof(long),1,fp);
    fseek(fp,-sizeof(tim),SEEK_END);
    rlong(tim);
    fseek(fp,0L,SEEK_SET);
    fread(&ch,1,1,fp);

#else

#define rint(i) read(fd,&i,sizeof(int));
#define rlong(i) read(fd,&i,sizeof(long));
    lseek(fd,-4,2);
    rlong(tim);
    lseek(fd,0,0);
    read(fd,&ch,1);

#endif
    if (sbuf.st_mtime > tim + 2 || sbuf.st_ctime > tim + 2 ||
      ch != MAGICNUM)
	BADF("Sorry, but the file has been touched.\n");
#ifdef __STDC__
    fread(&vers,sizeof(float),1,fp);
#else
    read(fd,&vers,sizeof(float));
#endif
    if (version != vers)
	BADF("Saved game is for the wrong version.\n");
#ifdef __STDC__
    overwrite(stdscr, getwin(fp));
    fread(mushw,24 * 57,1,fp);
#else
    for (y = 0; y < 24; y++)
	read(fd,(stdscr->_y)[y],80);
    read(fd,mushw,24 * 57);
#endif

    rint(numpedes);
    rint(fired);
    rlong(score);
    rint(board);
    rint(extramen);
    rlong(nextman);
    rint(dead);
    rint(finished);
    rint(breeding);
    rint(breedtime);
    rint(moves);
    rint(fleahere);
    rint(fleashot);
    rint(nummushrooms);
    rint(scorphere);
    rint(scorpthisboard);
    rint(scorpvel);
    rint(spiderhere);
    rint(spiderdir);
    rint(spidcount);

#define rc(cd) read(fd,&cd,sizeof(COORD))

    rc(guy);
    rc(shot);
    rc(flea);
    rc(scorp);
    rc(spider);
    rc(spidervel);
    read(fd,name,10);
    for (n = 0; n < numpedes; n++)
    {
	*piece = (PEDE *)malloc(sizeof(PEDE));
	read(fd,*piece,sizeof(PEDE));
	(*piece)->prev = prev;
	prev = *piece;
	piece = &(*piece)->next;
    }
    lastpede = prev;
    *piece = NULL;
#ifdef __STDC__
    fclose(fp);
#endif
    close(fd);
    unlink(fil);
    printf("Done\n");
    if (scorphere)
	scorppic = (scorpvel > 0) ? rscorpion : lscorpion;
    move(10,60);
    clrtoeol();
    move(11,60);
    clrtoeol();
    move(13,70);        /* Put the cursor in the right spot */
    redrawscr();        /* Redraw screen (necessary because of the way
			   the screen was put back together) */
}
