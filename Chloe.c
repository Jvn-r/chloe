#include<stdio.h>
#include<windows.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<unistd.h>
#include<wchar.h>

#define OP_SEMI 0
#define OP_AND  1
#define OP_OR   2
#define OP_PIPE 3
#define NOT_A_BUILTIN 195

typedef enum{
    TOK_WORD,
    TOK_OPERATOR
}TOK_TYPE;

struct tok{
    wchar_t *tok_word;
    TOK_TYPE type;
    int op_index; //if tok is not op then op_index = -1
};

struct tok TOKENS[10];
int used_tokens;


struct TOK_OPS{
    wchar_t *tok_op;
};
struct writing{
    wchar_t *buff;
    size_t cap;
    size_t pos;
    bool overflow;
};
struct inb{
    wchar_t *name;
    int (*fn_name)(wchar_t *argv[], struct writing *writ);
};


void scribble(wchar_t *buff, struct writing *writ){
    size_t buff_siz = wcslen(buff);
    if (writ->pos+buff_siz +1 <= writ->cap){
        wcsncat(writ->buff,buff,buff_siz);
        writ->pos += buff_siz;
        writ->buff[writ->pos] = L'\0'; 
    }
}

int create_proc(wchar_t *argv[], struct writing *writ){
    DWORD exit_code = 1;
    STARTUPINFOW si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    
    wchar_t cmd_line[500] = {0};
    int size_of_line;
    for (int i = 0; argv[i]; i++) {
        size_of_line = sizeof(cmd_line) - wcslen(cmd_line) - 1;
        wcsncat(cmd_line, argv[i], size_of_line);
        if (argv[i + 1])
            wcscat(cmd_line, L" ");
    }
    if (CreateProcessW(NULL,cmd_line,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi)){
        WaitForSingleObject(pi.hProcess,INFINITE);
        GetExitCodeProcess(pi.hProcess, &exit_code);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else{
        scribble(L"command doesnt exist\r\n", writ);
    }
    return exit_code;
}

void remove_new_line(wchar_t *buffer){
    buffer[wcscspn(buffer,L"\r\n")] = L'\0';
}

struct TOK_OPS operators[4] = {{L";"}, {L"&&"}, {L"||"}, {L"|"}};

int is_op(wchar_t x){
    for(int i=0 ; i<4 ; i++){
        if (x == operators[i].tok_op[0]){
            return 1;
        }
    }
    return 0;
}

int ope(wchar_t one, wchar_t two){
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

void tokenizer(wchar_t *buffer){
    for (int i = 0; i < 10; i++) {
        TOKENS[i].type = TOK_WORD;
        TOKENS[i].tok_word = NULL;
        TOKENS[i].op_index = -1;
    }
    used_tokens = 0;
    char cur_char, next_char;
    int i = 0, cur_ind, x;
    size_t sze = wcslen(buffer);

    for(size_t p = 0 ; p < sze ; p++){
        cur_char = buffer[p];

        if(cur_char == L'\0')
            break;

        if(buffer[p+1] != L'\0')
            next_char = buffer[p+1];
        else 
            next_char = L'\0';
        x = ope(cur_char, next_char);

        if (cur_char == L' ')//whitespace eater
            continue;
        else if(x != -1){ //operator gurgler
            TOKENS[i].op_index = x;
            TOKENS[i].type = TOK_OPERATOR;
            TOKENS[i].tok_word = NULL;

            if(TOKENS[i].type == TOK_OPERATOR && operators[TOKENS[i].op_index].tok_op[1] != L'\0')
                p++;
            i++;
        }  
        else{ 
            cur_ind = p;

            while(true){
                if (buffer[cur_ind] == L'\0' || buffer[cur_ind] == L' ') 
                    break;
                if (is_op(buffer[cur_ind])) 
                    break;
                if (buffer[cur_ind + 1] != L'\0' && ope(buffer[cur_ind], buffer[cur_ind + 1]) != -1) 
                    break;
                cur_ind++;
            }
            
            int cur_chr_stat = is_op(buffer[cur_ind]);
            wchar_t saved_o = L'\0', saved_c = L'\0';
            if(cur_chr_stat == 1){
                saved_o = buffer[cur_ind];
                buffer[cur_ind] = L'\0';
            }else if(buffer[cur_ind] == L'\0')
                {}
            else if(buffer[cur_ind] == L' '){
                buffer[cur_ind] = L'\0';
            }else{
                saved_c = buffer[cur_ind+1];
                buffer[cur_ind+1] = L'\0';
            }

            int len = cur_ind - p;
            wchar_t *word = malloc(len * sizeof(word[0]));
            if (!word)
                exit(1);

            wmemcpy(word, &buffer[p], len); 
            word[len] = L'\0';
            
            TOKENS[i].tok_word = word;
            TOKENS[i].type = TOK_WORD;
            TOKENS[i].op_index = -1;
            i++;

            if(saved_o != L'\0')
                buffer[cur_ind] = saved_o;
            else if(saved_c != L'\0')
                buffer[cur_ind+1] = saved_c;
            else
                {}
            if(buffer[cur_ind] == L' ' || buffer[cur_ind] == L'\0'){
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

int is_input_good(struct writing *writ){
    if(used_tokens == 0)
        return 0;

    if(TOKENS[0].type == TOK_OPERATOR){
        scribble(L"SYNTAX ERROR", writ);
        return -1;
    }
    
    if(TOKENS[used_tokens-1].type == TOK_OPERATOR){
        scribble(L"SYNTAX ERROR", writ);
        return -1;
    }
    for (int i = 0; i < used_tokens - 1; i++) {
        if (TOKENS[i].type == TOK_OPERATOR && TOKENS[i + 1].type == TOK_OPERATOR){
            scribble(L"SYNTAX ERROR", writ);
            return -1;
        }
    }
    return 1;
}

struct command{
    wchar_t *argv[50];
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

int inb_echo(wchar_t *argv[], struct writing *writ) {
    int i = 1;
    while(argv[i] != NULL){
        if(i==1){
            //wprintf(L"%ls",argv[i]);
            scribble(argv[i],writ);
        }else{
            //wprintf(L" %ls",argv[i]);
            scribble(L" ", writ);
            scribble(argv[i],writ);
        }
        i++;
    }
    //wprintf(L"\r\n");
    scribble(L"\r\n", writ);
    return 0;
}

int inb_pwd(wchar_t *argv[], struct writing *writ) {
    int size = GetCurrentDirectoryW(0, NULL);
    wchar_t *buffer;
    if (size == 0){
        //wprintf(L"error");
        scribble(L"pwd Error", writ);
        return 1;
    }
    else
        buffer = malloc(size);
    GetCurrentDirectoryW(size, buffer);
    //wprintf(L"%ls\r\n",buffer);
    scribble(buffer,writ);
    scribble(L"\r\n", writ);
    free(buffer);
    return 0;
}

int inb_cd(wchar_t *argv[], struct writing *writ){
    BOOL x = FALSE;
    if (argv[1]==NULL){
        //wprintf(L"\r\nGOING TO PWD\r\n");//D BUG
        return inb_pwd(argv, writ);
    }else{
        x = SetCurrentDirectoryW(argv[1]);
        //wprintf(L"\r\nAfter SETCURDIR setting X to tru or false\r\n");
    }
    if (x){
        //wprintf(L"\r\nIt thinks X is true\r\n");
        return 0;
    }    
    else{
        //wprintf(L"error\r\n");
        scribble(L"error\r\n", writ);
        return 1;
    }
}

int inb_hello(wchar_t *argv[], struct writing *writ){
    scribble(L"Hallo! This is Chloe, a custom Windows terminal\r\n Do help for list of in built functions\r\n", writ);
    return 0;
}

int inb_ver(wchar_t *argv[], struct writing *writ){
    wchar_t cur_ver[] = L"v0.1.1-alpha";
    scribble(L"Chloe-",writ);
    scribble(cur_ver,writ);
    scribble(L"\r\n",writ);
    return 0;
}

int inb_help(wchar_t *argv[], struct writing *writ);

// Excluded from the inbuilts for now, will make it wokr later, wont be forgotten
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
    return -1;
}

struct inb InBUILTS[] = {{L"echo", inb_echo}, {L"cd", inb_cd}, {L"pwd",inb_pwd}, {L"exit",inb_exit}, {L"ver",inb_ver}, {L"hello",inb_hello}, {L"help",inb_help}};

int inb_help(wchar_t *argv[], struct writing *writ){
    int size = sizeof(InBUILTS) / sizeof(InBUILTS[0]);
    wchar_t num[20];
    _itow_s(size, num, _countof(num), 10);
    scribble(L"Listing ", writ);
    scribble(num, writ);
    scribble(L" in built funcs\r\n", writ);

    //wprintf(L"Listing %d in built fucntions\r\n", size);
    for(int i=0; i<size ; i++){
        //wprintf(L"%ls\r\n", InBUILTS[i].name);
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

int run_cmd(wchar_t *argv[], struct writing *writ) {
    int stat = in_built_check(argv, writ);
    if (stat == -1)
        return -1;        
    if (stat != NOT_A_BUILTIN)
        return stat;        
    return create_proc(argv, writ); 
}

int executionar(int num_of_commds, struct command *cmds, struct writing *writ){
    int stat;
    for(int i=0; i<num_of_commds; i++){
        stat = run_cmd(cmds[i].argv, writ);
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

wchar_t *call_chloe(wchar_t *buff, wchar_t *op_buff, size_t cap){
    struct command cmds[10];
    //wprintf(L"wchar_t buff = %ls\r\n",buff); //D BUG
    
    struct writing writ;
    writ.buff = op_buff;
    writ.buff[0] = L'\0';
    writ.cap = cap;
    writ.pos = 0;
    writ.overflow = false;
    
    tokenizer(buff);
    //wprintf(L"1st tok = %ls\r\n",TOKENS[0].tok_word); //D BUG

    int inp_stat = is_input_good(&writ);
    if(inp_stat == -1){
        return writ.buff;
    }
    else{
        int no_cmds = command_builder(cmds);
        executionar(no_cmds, cmds, &writ);
        free_tokens(); 
        return writ.buff;
    }
}
