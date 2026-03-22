#ifndef CHLOE_H
#define CHLOE_H
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

#define OP_SEMI 0
#define OP_AND  1
#define OP_OR   2
#define OP_PIPE 3
#define NOT_A_BUILTIN 195
#define CHLOE_VERSION L"0.1.1-alpha"

typedef enum{
    TOK_WORD,
    TOK_OPERATOR
}TOK_TYPE;

struct tok{
    wchar_t *tok_word;
    TOK_TYPE type;
    int op_index; //if tok is not op then op_index = -1
};

extern struct tok TOKENS[64];
extern int used_tokens;

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
struct command{
    wchar_t *argv[50];
    int next_op_idx;
};

wchar_t *call_chloe(wchar_t *buff, wchar_t *op_buff, size_t capacity);

void scribble(wchar_t *buff, struct writing *writ);

#endif 