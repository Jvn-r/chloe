#include<stdio.h>
#include<windows.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>

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

struct TOK_OPS{
    char *tok_op;
};

struct TOK_OPS operators[4] = {{";"}, {"&&"}, {"||"}, {"|"}};

typedef enum{
    TOK_WORD,
    TOK_OPERATOR
}TOK_TYPE;

struct tok{
    char *tok_word;
    TOK_TYPE type;
    int op_index; //if tok is not op then op_index = -1
};

struct tok TOKENS[10];
int used_tokens;

int ope(char one, char two){
    int sze = sizeof(operators) / sizeof(operators[0]);
    for(int i = 0 ; i < sze ; i ++){
        if (operators[i].tok_op[1] != '\0'){
            if(one == operators[i].tok_op[0] && two == operators[i].tok_op[1])
                return i;
        }
        else{
            if((one == operators[i].tok_op[0]))
                return i;
        }
    }
    return -1;
}

void tokenizer(char *buffer){
    for (int i = 0; i < 10; i++) {
        TOKENS[i].type = TOK_WORD;
        TOKENS[i].tok_word = NULL;
        TOKENS[i].op_index = -1;
    }
    used_tokens = 0;

    char cur_char, next_char;
    char *cur_word;
    int i = 0, cur_ind, start, x;
    int sze = strlen(buffer);

    for(int p = 0 ; p < sze ; p++){
        cur_char = buffer[p];
        if(cur_char == '\0')
            break;

        if(buffer[p+1] != '\0')
            next_char = buffer[p+1];
        else 
            next_char = '\0';

        x = ope(cur_char, next_char);

        if (cur_char == ' ')//whitespace eater
            continue;
        else if(x != -1){ //operator gurgler
            TOKENS[i].op_index = x;
            TOKENS[i].type = TOK_OPERATOR;
            TOKENS[i].tok_word = NULL;

            if(TOKENS[i].type == TOK_OPERATOR && operators[TOKENS[i].op_index].tok_op[1] != '\0')
                p++;

            if(TOKENS[i].type == TOK_OPERATOR)
                i++;
        }   
        else{ 
            cur_ind = p;
            while(true){
                if (buffer[cur_ind] == '\0' ||buffer[cur_ind] == ' ' ||(buffer[cur_ind + 1] != '\0' && ope(buffer[cur_ind], buffer[cur_ind + 1]) != -1))
                    break;
                cur_ind++;
            }
            start = p;
            if(buffer[cur_ind] != '\0')
                buffer[cur_ind] = '\0';
            
            TOKENS[i].tok_word = &buffer[start];
            TOKENS[i].type = TOK_WORD;
            TOKENS[i].op_index = -1;
            i++;
            p = cur_ind;
        } 
    }
    used_tokens = i;
}

int is_input_good(struct tok *TOKENS){
    if(used_tokens == 0)
        return 0;

    if(TOKENS[0].type == TOK_OPERATOR)
        return -1;
    
    if(TOKENS[used_tokens-1].type == TOK_OPERATOR)
        return -1;

    for (int i = 0; i < used_tokens - 1; i++) {
        if (TOKENS[i].type == TOK_OPERATOR && TOKENS[i + 1].type == TOK_OPERATOR)
            return -1;
    }

    return 1;
}

void argv_builder(char *argv[]){
    int j=0;
    for(int i=0 ; i<used_tokens ; i++){
        if(TOKENS[i].type == TOK_OPERATOR)
            break;
        argv[j++] = TOKENS[i].tok_word;
    }
    argv[j] = NULL;
}

/*void old_tokenizer(char *buffer, char *argv[]){    
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
*/
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
    int inp_health;
    while(1){
        reading(gonk,100);
        remove_new_line(gonk);
        strcpy(raw_cmd,gonk);
        tokenizer(gonk);
        argv_builder(argv);
        //command_builder(argv);
        inp_health =  is_input_good(TOKENS);
        if(inp_health == -1){
            printf("syntax error\n");
            continue;
        }   
        else {
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
}