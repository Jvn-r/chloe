#include<stdio.h>
#include<windows.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<wchar.h>
#include "in_built.h"
#include "Chloe.h"

struct tok TOKENS[64];
int used_tokens;

void scribble(wchar_t *buff, struct writing *writ){
    size_t buff_siz = wcslen(buff);
    if (writ->pos+buff_siz +1 <= writ->cap){
        wcsncat(writ->buff,buff,buff_siz);
        writ->pos += buff_siz;
        writ->buff[writ->pos] = L'\0'; 
    }
    else{
        writ->overflow = true; // will make this impact the lore later
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
        if (exit_code!=0){
            scribble(L"Create Process exited with exit code : ",writ);
            swprintf(writ->buff + writ->pos, writ->cap, L"%lu\r\n", (unsigned long)exit_code);
        }
        scribble(L"command doesnt exist \r\n", writ);
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
    wchar_t cur_char, next_char;
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
            wchar_t *word = malloc((len+1) * sizeof(word[0]));
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
        scribble(L"SYNTAX ERROR\r\n", writ);
        return -1;
    }
    
    if(TOKENS[used_tokens-1].type == TOK_OPERATOR){
        scribble(L"SYNTAX ERROR\r\n", writ);
        return -1;
    }
    for (int i = 0; i < used_tokens - 1; i++) {
        if (TOKENS[i].type == TOK_OPERATOR && TOKENS[i + 1].type == TOK_OPERATOR){
            scribble(L"SYNTAX ERROR\r\n", writ);
            return -1;
        }
    }
    return 1;
}

int command_builder(struct command *cmds){
    int cmd_i = 0;
    int argv_j = 0;

    for(int i = 0; i<used_tokens; i++){
        if(TOKENS[i].type == TOK_WORD)
            cmds[cmd_i].argv[argv_j++] = TOKENS[i].tok_word;

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
    
    struct writing writ;
    writ.buff = op_buff;
    writ.buff[0] = L'\0';
    writ.cap = cap;
    writ.pos = 0;
    writ.overflow = false;
    
    tokenizer(buff);

    int inp_stat = is_input_good(&writ);
    if(inp_stat == -1){
        return writ.buff;
    }
    else{
        int no_cmds = command_builder(cmds);
        int stat = executionar(no_cmds, cmds, &writ);
        if (stat == -1){
            free_tokens();
            exit(0); 
        }
        free_tokens(); 
        return writ.buff;
    }
}
