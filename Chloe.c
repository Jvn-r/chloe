#include<stdio.h>
#include<windows.h>
#include<string.h>

int create_proc(char cmd_inp[]){
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    //char cmd_inp[] = "notepad.exe";

    if (CreateProcessA(NULL,cmd_inp,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi)){
        printf("oka it did it");
        WaitForSingleObject(pi.hProcess,INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else{
        printf("Iss not done");
    }
    return 0;
}

char *reading(char *buffer,int size){
    printf("\n$>");
    fgets(buffer,size,stdin);
    printf("BUFFER FILLED\n");
    return buffer;
}

char *remove_new_line(char *buffer){
    buffer[strcspn(buffer,"\n")] = '\0';
    return buffer;
}

char *tokenizer(char *buffer, char *argv[]){    
    int i = 1;
    char *tok;
    tok = strtok(buffer, " ");
    if (tok == NULL){
        return NULL;
    }
    argv[0] = tok;
    printf("\n tok of 1 is don \n");

    while(tok != NULL){
        argv[i] = tok;
        tok = strtok(NULL," ");
        printf("iteration %d \n",i);
        i++;
    }
    argv[i] = NULL;
    printf("Null added to end\n");
    return NULL;
}

int main(void){
    char gonk[100];
    char *argv[50];
    /*reading(gonk,100);
    printf("GONK REciveded\n");
    printf("%s",gonk);
    remove_new_line(gonk);    
    tokenizer(gonk,argv);
    printf("ARGV RETURNED\n");
    int j = 0;
    while(argv[j] != NULL){   
        printf("args = %s\n",argv[j]);
        j++;
    }
    create_proc(gonk);
    */
    while(1){
        reading(gonk,100);
        remove_new_line(gonk);
        tokenizer(gonk,argv);
        create_proc(gonk);
    }
}