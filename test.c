#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define maximum_Varname 20 // Maximum lenght in chars
#define maximum_Tokens 400
#define bufferForFiles 4000
#define FALSE 0 
#define TRUE 1

#include"TokenStack.h"

 /*Verifying pairs of [] () {}*/
int verifyPairs(TokenStack *IN, TokenStack *AUX)
{
    int i,j,k,d;

    initTokenStack(AUX);
    for(i=IN->initial;i<IN->initial+IN->items;i++)
    {
        j=IN->tokens[i].operator.type;
        if(j>100)
        {
            if(j>105)
            {
                k=!emptyTokenStack(AUX);
                d=topTokenStack(AUX).operator.type+5;
                if(k && d==j)
                    popTokenStack(AUX);
                else if(k )
                {
                    printf("Error: Uncorresponding pairs ``´´.");
                    return FALSE;
                }
                else
                {
                    printf("Error: An additional and unexpected closing symbol ``´´");
                    return FALSE;
                }

            }
            else
               pushTokenStack(AUX,IN->tokens[i]);
        }
    }
    if(emptyTokenStack(AUX))
        return TRUE;
    printf("Error: Expecting a closing symbol ``´´ corresponding to ``´´ [].\n");
    return FALSE;

}

int process(TokenStack *IN, TokenStack *OUT)
{
    TokenStack AUX;
    Token tkn;
    int i,j;

    // Error for dissmaching [] () {}
    if(!verifyPairs(IN,&AUX))
        return 1;
    
    initTokenStack(&AUX);
    while(!emptyTokenStack(IN))
    {  
        tkn=popTokenStack(IN);
        if(tkn.operand.type<=0)
        {
            pushTokenStack(OUT,tkn);
            printf("\t%s\n",tkn.operand.txtReference);
        }
        else
        {
            printf("\t%i\n",tkn.operator.type);

            if(emptyTokenStack(&AUX))
            {
                pushTokenStack(&AUX,tkn);
                continue;
            } 

            //Code of this token
            i=tkn.operator.type;
            do{
                // Code of the top of AUX
                j=topTokenStack(&AUX).operator.type;

                if(j/10<=i/10)
                    pushTokenStack(OUT,popTokenStack(&AUX));
                else // j/10>i/10
                    break;      
            }while(!emptyTokenStack(&AUX)); //>
            pushTokenStack(&AUX,tkn);
        }
    }
    while(!emptyTokenStack(&AUX))
        pushTokenStack(OUT,popTokenStack(&AUX));

    return 0;   
}

int main(int argc, char* argv[])
{
    TokenStack IN, OUT;

    printf("\n\n[%i]\n\n",
    loadFile("Example.txt", &IN));
    process(&IN,&OUT);

    int i,j;
    Token t;

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