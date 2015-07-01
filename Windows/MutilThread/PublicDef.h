#ifndef _PUBLIC_DEF_
#define _PUBLIC_DEF_

#include <Windows.h>
#include <stdio.h>

struct Msg
{
	int head;
	int body;
	char words[BUFSIZ];
};
#define MaxQueSize  100

#endif
