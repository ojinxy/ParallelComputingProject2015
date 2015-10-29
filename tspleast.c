
static char rcsid[] =
	"$Id: hello_other.c,v 1.3 2009/01/23 01:12:48 pvmsrc Exp $";



#include <stdio.h>
#ifdef HASSTDLIB
#include <stdlib.h>
#endif
#ifdef	SYSVSTR
#include <string.h>
#else
#include <strings.h>
#endif
#include "pvm3.h"

main()
{
	printf("Starting Main TSP Least");
	int ptid;
	char buf[100];

	ptid = pvm_parent();

	strcpy(buf, "hello, world from ");
	gethostname(buf + strlen(buf), 64);

	pvm_initsend(PvmDataDefault);
	pvm_pkstr(buf);
	pvm_send(ptid, 1);

	pvm_exit();
	exit(0);
}

