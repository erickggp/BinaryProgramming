#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define maximum_Varname 20 // Maximum lenght in chars
#define maximum_Tokens 400
#define bufferForFiles 4000
#define FALSE 0 
#define TRUE 1

#include"TokenStack.h"
#include"Prefix.h"


int main(int argc, char* argv[])
{
    TokenStack OUT;
    Token t;

    if(argc<2)
        return 0;

    if(fileToPrefix(argv[1],&OUT))
        return 0;
    
    while(!emptyTokenStack(&OUT))
    {
        t=popTokenStack(&OUT);
        if(t.operator.type>0)
            printf("[%u,%u] OPERATOR: %u.\n",
            t.operator.line,t.operator.column,
            t.operator.type);
        else if(t.operand.type==0)
            printf("[%u,%u] VARIABLE: %s.\n",
            t.operand.line,t.operand.column,
            t.operand.txtReference);
        else if(t.operand.type==-1)
            printf("[%u,%u] INTEGER: %lli.\n",
            t.operand.line,t.operand.column,
            t.operand.content.integer);
        else if(t.operand.type==-1)
            printf("[%u,%u] INTEGER: %lf.\n",
            t.operand.line,t.operand.column,
            t.operand.content.real);
    }
    return 0;
}