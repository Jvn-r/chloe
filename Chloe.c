#include<stdio.h>
#include<windows.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>

#define OP_SEMI 0
#define OP_AND  1
#define OP_OR   2
#define OP_PIPE 3
#define NOT_A_BUILTIN 195

int create_proc(char *argv[]){
    DWORD exit_code = 1;
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    
    char cmd_line[500] = {0};
    int size_of_line;
    for (int i = 0; argv[i]; i++) {
        size_of_line = sizeof(cmd_line) - strlen(cmd_line) - 1;
        strncat(cmd_line, argv[i], size_of_line);
        if (argv[i + 1])
            strcat(cmd_line, " ");
    }
    printf("\n FINAL cEAT PRIRND STRING  =  %s \n" , cmd_line);
    if (CreateProcessA(NULL,cmd_line,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi)){
        WaitForSingleObject(pi.hProcess,INFINITE);
        GetExitCodeProcess(pi.hProcess, &exit_code);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else{
        printf("Iss not done");
    }
    return exit_code;
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

int is_op(char x){
    for(int i=0 ; i<4 ; i++){
        if (x == operators[i].tok_op[0]){
            printf(" from is_op START OF OPERAOTN = %c\n",operators[i].tok_op[0]);
            return 1;
        }
    }
    return 0;
}

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
    printf("\nTKONATOR\n");
    char cur_char, next_char;
    char *cur_word;
    int i = 0, cur_ind, start, x;
    int sze = strlen(buffer);

    for(int p = 0 ; p < sze ; p++){
        printf("ITERATION %d\n",p);
        cur_char = buffer[p];
        printf(" cur char = %c ---", cur_char);
        if(cur_char == '\0')
            break;

        if(buffer[p+1] != '\0')
            next_char = buffer[p+1];
        else 
            next_char = '\0';

        printf(" next char = %c ---",next_char);

        x = ope(cur_char, next_char);

        printf(" \n output of ope(%c,%c) is %d \n", cur_char, next_char, x);

        if (cur_char == ' ')//whitespace eater
            continue;
        else if(x != -1){ //operator gurgler

            printf("\nENTERING OPE GURGUELS\n");
            printf(" buffer[%d] = %c ", p,buffer[p]);
            TOKENS[i].op_index = x;
            TOKENS[i].type = TOK_OPERATOR;
            TOKENS[i].tok_word = NULL;

            if(TOKENS[i].type == TOK_OPERATOR && operators[TOKENS[i].op_index].tok_op[1] != '\0')
                p++;
            i++;
            printf("I = %d\n", i);
            printf("P = %d\n", p);
        }  
        else{ 
            printf(" \n WORD WORD WOKD \n");
            printf(" the index is at %d \n" , p);
            cur_ind = p;

            int iter = 0;

            while(true){

                printf("in while loop iteration %d \n",iter);
                iter ++;
                if (buffer[cur_ind] == '\0' || buffer[cur_ind] == ' ') 
                    break;
                
                if (is_op(buffer[cur_ind])) 
                    break;
                
                if (buffer[cur_ind + 1] != '\0' && ope(buffer[cur_ind], buffer[cur_ind + 1]) != -1) 
                    break;
                
                printf("CURRENT INDEX VAR = %d\n", cur_ind);
                cur_ind++;
                
            }
            printf("buffer[%d] is  = %c ", cur_ind, buffer[cur_ind]);
            
            int cur_chr_stat = is_op(buffer[cur_ind]);
            char saved_o = '\0', saved_c = '\0';
            if(cur_chr_stat == 1){
                printf("AAAAAAAA buff[cur_index = %d] = %c \n", cur_ind, buffer[cur_ind]);
                saved_o = buffer[cur_ind];
                printf(" OP saved = %c \n", saved_o);
                buffer[cur_ind] = '\0';
            }else if(buffer[cur_ind] == '\0'){
                printf("NULL HCHCHCHC \n");
            }else if(buffer[cur_ind] == ' '){
                buffer[cur_ind] = '\0';
                printf("\nWHITSTSTSTS %c ", buffer[cur_ind]);
            }else{
                printf("AAAAAAAA THE BUFFER AT CUR_IND IS A LETTER = %c\n", buffer[cur_ind]);
                saved_c = buffer[cur_ind+1];
                printf("CHAR SAVED = %c\n", saved_c);
                buffer[cur_ind+1] = '\0';
            }
            printf("\n TONE OF THESE SHOULD BE NULL %c, %c \n",buffer[cur_ind], buffer[cur_ind+1]);
            //if(buffer[cur_ind] != '\0')
            //    buffer[cur_ind] = '\0';
            //---
            int len = cur_ind - p;
            char *word = malloc(len + 1);
            if (!word) {
                perror("malloc aaaaaaaaaa");
                exit(1);
            }
            memcpy(word, &buffer[p], len);
            word[len] = '\0';
            
            TOKENS[i].tok_word = word;
            //---
            TOKENS[i].type = TOK_WORD;
            TOKENS[i].op_index = -1;
            i++;

            if(saved_o != '\0')
                buffer[cur_ind] = saved_o;
            else if(saved_c != '\0')
                buffer[cur_ind+1] = saved_c;
            else
                printf("\nITS WHITESPCEA AND IT SHOULD BE FINEEE\n");

            printf(" the index AT THE END is at %d \n" , p);
            if(buffer[cur_ind] == ' ' || buffer[cur_ind] == '\0'){
                p = cur_ind;
                printf("\n buff at cur ind is WHITESPACE or NULL char so p = cur index = %d", p);
            }
            else{
                p = cur_ind-1;
                printf("\n buff at cur ind is NOT NOT NOT A WHITESPACE or NULL char so p = cur index -1 = %d", p);
            }
            printf(" p AFTR p=cur_ind IS  %d \n" , p);
        } 
    }
    used_tokens = i;
}

void free_tokens() {
    for (int i = 0; i < used_tokens; i++) {
        if (TOKENS[i].type == TOK_WORD && TOKENS[i].tok_word) {
            free(TOKENS[i].tok_word);
            TOKENS[i].tok_word = NULL;
        }
    }
}

int is_input_good(){
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

struct command{
    char *argv[50];
    int next_op_idx;
};

int command_builder(struct command *cmds){
    int cmd_i = 0;
    int argv_j = 0;

    for(int i = 0; i<used_tokens; i++){
        if(TOKENS[i].type == TOK_WORD){
            cmds[cmd_i].argv[argv_j++] = TOKENS[i].tok_word;
        }
        if(TOKENS[i].type == TOK_OPERATOR){
            cmds[cmd_i].argv[argv_j] = NULL;
            cmds[cmd_i].next_op_idx = TOKENS[i].op_index;
            cmd_i++;
            argv_j = 0;
        }
    }
    cmds[cmd_i].argv[argv_j] = NULL;
    cmds[cmd_i].next_op_idx = -1;
    return cmd_i + 1;;
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
    return 0;
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
    return 0;
}

int inb_cd(char *argv[]) {
    BOOL x = FALSE;
    if (argv[1]==NULL)
        return inb_pwd(argv);
    else
        x = SetCurrentDirectory(argv[1]);
    
    if (x)
        return 0;
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
    int count = sizeof(InBUILTS) / sizeof(InBUILTS[0]);
    for(int i=0; i<count;i++){
        if(strcmp(argv[0],InBUILTS[i].name)==0){
            return InBUILTS[i].fn_name(argv);
        }
    }   
    return 195;
}

int run_cmd(char *argv[]) {
    int stat = in_built_check(argv);
    if (stat == -1)
        return -1;        
    if (stat != NOT_A_BUILTIN)
        return stat;        

    return create_proc(argv); 
}

int executionar(int num_of_commds, struct command *cmds){
    int stat;
    for(int i=0; i<num_of_commds; i++){
        stat = run_cmd(cmds[i].argv);
        if(stat == -1)
            return -1;

        if(cmds[i].next_op_idx == -1)
            continue;
        if(cmds[i].next_op_idx == OP_SEMI)
            continue;
        if (cmds[i].next_op_idx == OP_AND && stat != 0)
            break;
        if (cmds[i].next_op_idx == OP_OR && stat == 0)
            break;
    }
    return 0;
}

int main(void){
    char gonk[100];
    char *argv[50];
    char raw_cmd[100];
    int inp_health, n_o_cmd;
    struct command cmds[10];
    while(1){
        reading(gonk,100);
        remove_new_line(gonk);
        strcpy(raw_cmd,gonk);
        tokenizer(gonk);

        for(int i = 0; i < used_tokens; i++){
            if(TOKENS[i].type == TOK_WORD)
                printf("Token %d: WORD '%s'\n", i, TOKENS[i].tok_word);
            else
                printf("Token %d: OPERATOR %d\n", i, TOKENS[i].op_index);
        }

        inp_health =  is_input_good();
        if(inp_health == -1){
            printf("syntax error\n");
            continue;
        }   
        else {
            n_o_cmd = command_builder(cmds);
            int inb_stat = executionar(n_o_cmd, cmds);
            free_tokens(); 
            if(used_tokens < 1)
                continue;
            if(inb_stat == -1)
                break;
            if(inb_stat == 0){
                printf("in built wokd\n");
                continue;
            }
        }
    }
}