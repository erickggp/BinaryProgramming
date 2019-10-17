/* Union for saving a simple value
 or a list address (pointer) */
typedef union
{
    long long int integer;
    double real;
    long long int *integers;
    double *reals;
} Content;

/* Structure for saving an operand.
"dimension" is the length of a list,
or well, 0 for a simple variable */
typedef struct
{
    short type;
    short unsigned line, column;
    unsigned dimension;
    char txtReference[maximum_Varname];
    Content content;
} Operand;

/* Structure for saving an operator*/
typedef struct
{   
    short type;
    short unsigned line, column;
} Operator;

/* Union for saving an operand or an 
operator, in other words, a token. */
typedef union
{
    Operand operand;
    Operator operator;
} Token;

/* Structure for managing a Stack of
Tokens */
typedef struct
{
    int initial, items;
    Token tokens[maximum_Tokens];
} TokenStack;

void initTokenStack(TokenStack *stk)
{
    stk->items=0;
    stk->initial=0;
}

unsigned operatorType(char* str)
{
    // ¡¡¡ALL CODES MUST BE > 0!!!
    // For each code, CODE/10 is the 
    // corresponding precedence.
    switch(str[0])
    {
        case '.':
            return 10;
        case ':':
            return 20;
        case '&':
            return 30;
        case '*':
            return 31;
        case '/':
            return 32;
        case '%':
            return 33;
        case '|':
            return 40;
        case '+':
            return 41;
        case '-':
            return 42;
        case '=':
            switch(str[1])
            {
                case '=':
                    return 52; // (==)
                case '/':
                    return 53; // (=/)
            }
            return 51; // (=)
        
        case '<':
            switch(str[1])
            {
                case '=':
                    return 55; // (<=)
                default:
                    return 54; // (<)
            }

        case '>':
            switch(str[1])
            {
                case '=':
                    return 57; // (>=)
                default:
                    return 56; // (>)
            }

        case '?':
            switch(str[1])
            {
                case '*':
                    return 81; // (??)
                default:
                    return 61; // (?)
            }

        case '!':
            switch(str[1])
            {
                case '*':
                    return 82; // (!!)
                default:
                    return 71; // (!)
            }

        case ',':
            return 90;
        
        case '(':
            return 101;
        case '[':
            return 102;
        case '{':
            return 103;

        case ')':
            return 106;
        case ']':
            return 107;
        case '}':
            return 108;
    }
    return 0;
}


char* isANumber(char* str, char *buff)
{
    int i,j=0,a;  

    a=0;
    if(str[0]=='-')
    {
        a=1;
        buff[0]='-';
    }
    

    /*Copying numbers and dots to buffer*/
    for(i=a; str[i]!='\0'; i++)
    {
        if(str[i]>='0' && str[i]<='9')
            buff[i]=str[i];
        else if(str[i]=='.')
        {
            buff[i]='.';
            j++;
        }
        else
            break;
    }
    buff[i]='\0';

    

    // If buff is a number, the str-index increases
    if(j<2 && i-j>a)
        return &str[i];

    return str;
}

char* isAnOperator(char* str, short* type)
{
    short unsigned x=operatorType(str);
    if(x==0)
        return str;
    (*type)=x;
    short unsigned j;
    /* Codes assigned by the function operatorType(str) */
    switch (x)
    {
        // Operators based on 1 char
        case 10:
        case 20:
        case 30:
        case 31:
        case 32:
        case 33:
        case 40:
        case 41:
        case 42:
        case 51:
        case 54:
        case 56:
        case 61:
        case 71:
        case 90:
        case 108:
        case 107:
        case 106:
        case 101:
        case 102:
        case 103:
            j=1;
            break;
 
        // Operators based on 2 chars    
        case 52: 
        case 53:
        case 55:
        case 57:
        case 81:
        case 82:
            j=2;
            break;

        default:
            j=0;
    }
    
    return &str[j];
}

char* isAVariable(char* str, char* buff)
{
    int i,j;

    j=0;
    if(str[j]=='-')
        buff[j++]='-';

    /* Looking for a string of chars without
    spaces: ' ', '\n' , ... */
    for(i=j; str[i]!='\0'; i++)
    {
        if(operatorType(&str[i]) ||
        str[i]=='\b' || str[i]=='\t' || 
        str[i]=='\n' || str[i]=='\r' ||
        str[i]=='\v' || str[i]=='\f' ||
        str[i]==' ')
            break;
        
        buff[i]=str[i];
    }
    buff[i]='\0';

    if(i-j>0)
        return &str[i];

    return str;
} 


int pushTokenStack(TokenStack *stk, Token tkn)
{
    if(stk->items<maximum_Tokens)
    {
        stk->initial=(stk->initial+maximum_Tokens-1)
                        %maximum_Tokens;
        stk->tokens[stk->initial]=tkn;
        stk->items++;
        return TRUE;
    }
    return FALSE;
}

Token popTokenStack(TokenStack *stk)
{
    Token res;
    if(stk->items>0)
    {
        res=stk->tokens[stk->initial];
        stk->initial=(stk->initial+1)%maximum_Tokens;
        stk->items--;
    }
    return res;
}

int emptyTokenStack(TokenStack *stk)
{
    if(stk->items>0)
        return FALSE;
    return TRUE;
}

Token topTokenStack(TokenStack *stk)
{
    return stk->tokens[stk->initial];
}