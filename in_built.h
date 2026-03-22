#ifndef IN_BUILT_H
#define IN_BUILT_H
#include "Chloe.h"

extern struct inb InBUILTS[];

int inb_echo(wchar_t *argv[], struct writing *writ);

int inb_pwd(wchar_t *argv[], struct writing *writ);

int inb_cd(wchar_t *argv[], struct writing *writ);

int inb_hello(wchar_t *argv[], struct writing *writ);

int inb_ver(wchar_t *argv[], struct writing *writ);

int inb_help(wchar_t *argv[], struct writing *writ);

int inb_exit(wchar_t *argv[], struct writing *writ);

int in_built_check(wchar_t *argv[], struct writing *writ);

#endif