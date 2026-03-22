#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include "Chloe.h"
#include "in_built.h"

int inb_echo(wchar_t *argv[], struct writing *writ) {
    int i = 1;
    while(argv[i] != NULL){
        if(i==1)
            scribble(argv[i],writ);
        else{
            scribble(L" ", writ);
            scribble(argv[i],writ);
        }
        i++;
    }
    scribble(L"\r\n", writ);
    return 0;
}

int inb_pwd(wchar_t *argv[], struct writing *writ) {
    int size = GetCurrentDirectoryW(0, NULL);
    wchar_t *buffer;
    if (size == 0){
        scribble(L"pwd Error\r\n", writ);
        return 1;
    }
    else
        buffer = malloc(size * sizeof(wchar_t));
    GetCurrentDirectoryW(size, buffer);
    scribble(buffer,writ);
    scribble(L"\r\n", writ);
    free(buffer);
    return 0;
}

int inb_cd(wchar_t *argv[], struct writing *writ){
    BOOL x = FALSE;
    if (argv[1]==NULL)
        return inb_pwd(argv, writ);
    else
        x = SetCurrentDirectoryW(argv[1]);
    if (x)
        return 0; 
    else{
        scribble(L"error\r\n", writ);
        return 1;
    }
}

int inb_hello(wchar_t *argv[], struct writing *writ){
    scribble(L"Hallo! This is Chloe, a custom Windows shell running on a simple Win32 UI\r\n Do help for list of in built functions\r\n", writ);
    return 0;
}

int inb_ver(wchar_t *argv[], struct writing *writ){
    scribble(L"Chloe-",writ);
    scribble(CHLOE_VERSION,writ);
    scribble(L"\r\n",writ);
    return 0;
}

int inb_help(wchar_t *argv[], struct writing *writ);

// calc is excluded from the inbuilts for now, will make it wokr later, wont be forgotten
int inb_calc(wchar_t *argv[]){
    int a,b;
    wchar_t op;
    while(true){
        wprintf(L"SImlpe Calc \r\n Please follow format : num1 op num2\r\n"); 
        wscanf(L"%d %lc %d",&a, &op, &b);
        switch(op){
            case '+': wprintf(L"\r\n%d", a+b); break;
            case '-': wprintf(L"\r\n%d",a-b);break;
            case '*': wprintf(L"\r\n%d", a*b); break;
            case '/': wprintf(L"\r\n%d", a/b); break;
            default: wprintf(L"please enter valid operator\r\n"); break;
        }
    }
    return 0;
}

int inb_exit(wchar_t *argv[], struct writing *writ){
    scribble(L"exiting...", writ);
    return -1;
}

struct inb InBUILTS[] = {{L"echo", inb_echo}, {L"cd", inb_cd}, {L"pwd",inb_pwd}, {L"exit",inb_exit}, {L"ver",inb_ver}, {L"hello",inb_hello}, {L"help",inb_help}};

int inb_help(wchar_t *argv[], struct writing *writ){
    int size = sizeof(InBUILTS) / sizeof(InBUILTS[0]);
    wchar_t num[20];

    _itow_s(size, num, _countof(num), 10); // int to size_t cast

    scribble(L"Listing ", writ);
    scribble(num, writ);
    scribble(L" in built funcs\r\n", writ);

    for(int i=0; i<size ; i++){
        scribble(InBUILTS[i].name, writ);
        scribble(L"\r\n",writ);
    }
    return 0;
}

int in_built_check(wchar_t *argv[], struct writing *writ){
    int count = sizeof(InBUILTS) / sizeof(InBUILTS[0]);
    for(int i=0; i<count;i++){
        if(wcscmp(argv[0],InBUILTS[i].name)==0){
            return InBUILTS[i].fn_name(argv, writ);
        }
    }   
    return 195;
}