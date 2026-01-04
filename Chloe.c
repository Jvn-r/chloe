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
        //printf("oka it did it");
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
    //printf("BUFFER FILLED\n");
    //return buffer;
}

void remove_new_line(char *buffer){
    buffer[strcspn(buffer,"\n")] = '\0';
    //return buffer;
}

void tokenizer(char *buffer, char *argv[]){    
    int i = 0;
    char *tok;
    //printf("TOK BUFFER BEFOGRE TOK : %s\n",buffer);
    tok = strtok(buffer, " ");
    //if (tok == NULL)
        //exit;
    //argv[0] = tok;
    //printf("\n tok of 1 is don \n");

    while(tok != NULL){
        argv[i] = tok;
        tok = strtok(NULL," ");
        printf("iteration %d \n",i);
        i++;
        if(i == 49)
            break;
    }
    argv[i] = NULL;
    //printf("Null added to end\n");
    //printf("TOK BUFFEEr %s\n",buffer);
}

int main(void){
    char gonk[100];
    char *argv[50];
    char raw_cmd[100];
    while(1){
        reading(gonk,100);
        //printf("reading %s\n",raw_cmd);
        remove_new_line(gonk);
        strcpy(raw_cmd,gonk);
        //printf("new line %s\n",gonk);
        tokenizer(gonk,argv);
        //printf("TOK %s\n",gonk);
        if(argv[0] == NULL)
            continue;
        if(strcmp(argv[0],"exit") == 0)
            break;
        create_proc(raw_cmd);
    }
}