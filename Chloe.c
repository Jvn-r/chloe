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
    if (CreateProcessA(NULL,cmd_line,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi)){
        WaitForSingleObject(pi.hProcess,INFINITE);
        GetExitCodeProcess(pi.hProcess, &exit_code);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else{
        printf("command doesnt exist");
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
    char cur_char, next_char;
    int i = 0, cur_ind, x;
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
            i++;
        }  
        else{ 
            cur_ind = p;

            while(true){
                if (buffer[cur_ind] == '\0' || buffer[cur_ind] == ' ') 
                    break;
                if (is_op(buffer[cur_ind])) 
                    break;
                if (buffer[cur_ind + 1] != '\0' && ope(buffer[cur_ind], buffer[cur_ind + 1]) != -1) 
                    break;
                cur_ind++;
            }
            
            int cur_chr_stat = is_op(buffer[cur_ind]);
            char saved_o = '\0', saved_c = '\0';
            if(cur_chr_stat == 1){
                saved_o = buffer[cur_ind];
                buffer[cur_ind] = '\0';
            }else if(buffer[cur_ind] == '\0')
                {}
            else if(buffer[cur_ind] == ' '){
                buffer[cur_ind] = '\0';
            }else{
                saved_c = buffer[cur_ind+1];
                buffer[cur_ind+1] = '\0';
            }

            int len = cur_ind - p;
            char *word = malloc(len + 1);
            if (!word)
                exit(1);

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
                {}
            if(buffer[cur_ind] == ' ' || buffer[cur_ind] == '\0'){
                p = cur_ind;
            }
            else{
                p = cur_ind-1;
                }
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

int inb_hello(char *argv[]){
    printf("Hallo! This is Chloe, a custom Windows terminal\n Do help for list of in built functions\n");
    return 0;
}

int inb_ver(char *argv[]){
    char cur_ver[] = "v0.1";
    printf("Chloe-%s\n",cur_ver);
    return 0;
}

int inb_help(char *argv[]);

int inb_exit(char *argv[]){
    return -1;
}

struct inb{
    char *name;
    int (*fn_name)(char *argv[]);
};
struct inb InBUILTS[] = {{"echo", inb_echo}, {"cd", inb_cd}, {"exit",inb_exit}, {"pwd",inb_pwd},{"ver",inb_ver},{"hello",inb_hello},{"help",inb_help}};

int inb_help(char *argv[]){
    int size = sizeof(InBUILTS) / sizeof(InBUILTS[0]);
    printf("Listing %d in built fucntions\n", size);
    for(int i=0; i<size ; i++){
        printf("%s\n", InBUILTS[i].name);
    }
}

int in_built_check(char *argv[]){
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
    char raw_cmd[100];
    int inp_health, n_o_cmd;
    struct command cmds[10];
    while(1){
        reading(gonk,100);
        remove_new_line(gonk);
        strcpy(raw_cmd,gonk);
        tokenizer(gonk);
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
                continue;
            }
        }
    }
}