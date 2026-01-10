#include<stdio.h>
#include<windows.h>
#include<string.h>
#include <stdlib.h>

int create_proc(char cmd_inp[]){
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    if (CreateProcessA(NULL,cmd_inp,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi)){
        WaitForSingleObject(pi.hProcess,INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else{
        printf("Iss not done");
    }
    return 0;
}

void reading(char *buffer,int size){
    printf("\n$>");
    fgets(buffer,size,stdin);
}

void remove_new_line(char *buffer){
    buffer[strcspn(buffer,"\n")] = '\0';
}

void tokenizer(char *buffer, char *argv[]){    
    int i = 0;
    char *tok;
    tok = strtok(buffer, " ");

    while(tok != NULL){
        argv[i] = tok;
        tok = strtok(NULL," ");
        i++;
        if(i == 49)
            break;
    }
    argv[i] = NULL;
}

int inb_echo(char *argv[]) {
    int i = 1;
    while(argv[i] != NULL){
        if(i==1)
            printf("%s",argv[i]);
        else
            printf(" %s",argv[i]);
        i++;
    }
    printf("\n");
    return 1;
}

int inb_cd(char *argv[]) {
    BOOL x = FALSE;
    if (argv[1]==NULL)
        return inb_pwd(argv);
    else
        x = SetCurrentDirectory(argv[1]);
    
    if (x)
        return 1;
    else{
        printf("error\n");
        return 1;
    }
}

int inb_pwd(char *argv[]) {
    int size = GetCurrentDirectory(0, NULL);
    char *buffer;
    if (size == 0){
        printf("error");
        return 1;
    }
    else
        buffer = malloc(size);
    GetCurrentDirectory(size,buffer);
    printf("%s\n",buffer);
    free(buffer);
    return 1;
}

int inb_exit(char *argv[]){
    return -1;
}

struct inb{
    char *name;
    int (*fn_name)(char *argv[]);
};

int in_built_check(char *argv[]){
    struct inb InBUILTS[4] = {{"echo", inb_echo}, {"cd", inb_cd}, {"exit",inb_exit}, {"pwd",inb_pwd}};
    int i;
    int count = sizeof(InBUILTS) / sizeof(InBUILTS[0]);
    for(i=0; i<count;i++){
        if(strcmp(argv[0],InBUILTS[i].name)==0){
            return InBUILTS[i].fn_name(argv);
        }
    }
    return 0;
}

int main(void){
    char gonk[100];
    char *argv[50];
    char raw_cmd[100];
    while(1){
        reading(gonk,100);
        remove_new_line(gonk);
        strcpy(raw_cmd,gonk);
        tokenizer(gonk,argv);
;
        if(argv[0] == NULL)
            continue;
        int inb_stat = in_built_check(argv);
        if(inb_stat == -1)
            break;
        if(inb_stat == 1){
            printf("in built wokd\n");
            continue;
        }
        create_proc(raw_cmd);
    }
}