#include "Chloe.h"
#include <stdio.h>

void main(){
    wchar_t buff[1024] = L"echo workin";
    wchar_t op[1024];
    call_chloe(buff,op);
    printf("%ls" ,op);
}