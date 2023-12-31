/*  Version 1.0		6/12/85
	Jim Hein
	Nicolet Instrument Corp
	5225 Verona Rd
	Madison, WI  53611-0288
*/
#
# include <fcntl.h>

# define CR	13
# define NL	10
# define CTRL_Z	26
# define BLOCK	512

DELCR(file)
char *file; {
	int id, oid;
	char buf[BLOCK+10], *strchr();
	static char temp[11];
	register char *ptr = buf, *od, *cz;
	register int len;

	strcpy(temp,"tempXXXXXX");
	mktemp(temp);
	if ((oid = open(temp,O_CREAT | O_WRONLY,0644)) < 0) {
		perror(temp);
		unlink(temp);
		exit(-1);
	}

	if ((id = open(file,O_RDONLY,0644)) < 0) {
		perror(file);
		unlink(temp);
		exit(-1);
	}

	while ((len = read(id,ptr,BLOCK)) > 0) {
		*(ptr + len) = 0;
		while ( od = strchr(ptr,CR) ) {
			*od = '\0';
			if (cz = strchr(ptr,CTRL_Z)) {
				*cz = '\0';
				write(oid, ptr, strlen(ptr));
			 	goto OUT;
			}
			write(oid, ptr, strlen(ptr));
			ptr = ++od;
		}
		if (cz = strchr(ptr,CTRL_Z)) {
			*cz = '\0';
			write(oid, ptr, strlen(ptr));
		 	goto OUT;
		}
		if (len = strlen(ptr))
		write(oid, ptr, len);
		ptr = buf;
	}

OUT:
	close(oid); close(id);
	strcpy(buf,"mv ");
	strcat(buf,temp);
	strcat(buf," ");
	strcat(buf,file);
	system(buf);
}
