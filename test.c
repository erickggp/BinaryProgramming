#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define maximum_Varname 20 // Maximum lenght in chars
#define maximum_Tokens 400
#define bufferForFiles 4000
#define FALSE 0 
#define TRUE 1

#include"TokenStack.h"

int main(int argc, char* argv[])
{
    TokenStack IN;

    printf("\n\n[%i]\n\n",
    loadFile("Example.txt", &IN));
    return 0;
}