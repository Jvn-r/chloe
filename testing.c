#include "Chloe.h"
#include <stdio.h>

void main(){
    wchar_t buff[1024] = L"hello";
    wchar_t op[1024];
    size_t cap = 1024;
    call_chloe(buff,op,cap);
    printf("%ls" ,op);
}