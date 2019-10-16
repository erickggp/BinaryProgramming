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

        case ']':
            return 106;
        case ')':
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

int pushTokenStack()
{
    return 0;
}

Token popTokenStack(TokenStack *stk)
{
    Token a;
    return a;
}


int loadFile(char* file_name, TokenStack* IN)
{
    //--Loads the file into the buffer--
    FILE* in = fopen(file_name,"r");

    // "Error: No file named %s.", file_name
    if(in==NULL)
        return 1;

    char buffer[bufferForFiles];
    unsigned size=0;
    char c;

    while(fscanf(in,"%c",&c) != EOF && size<bufferForFiles)
        buffer[size++]=c;
    
    // Error: Program too long
    if(size>=bufferForFiles)
        return 2;

    buffer[size++]=' ';
    buffer[size++]='\0';
    //----------------------------------


    Token *tkn;
    char* index;
    char* aux;
    unsigned i,j,k;
    unsigned line=1;
    unsigned column=0;

    for(i=0;i<size-1;i++)if(buffer[i]=='-')
    if(buffer[i+1]=='(' || buffer[i+1]=='[' || buffer[i+1]=='{')
    {
        for(j=size-1;j>i;j--)
            buffer[j+2]=buffer[j];
        buffer[i+1]='1';
        buffer[i+2]='*';
        size+=2;
    }

    printf("%s\n",buffer);

    j=0;
    for(i=0;i<size-1;i++)
    {
        column++;

        /* For ingoring commentaries */
        if(buffer[i]=='\\')
            for(;buffer[i]!='\n';i++);


        /* Counting lines and columns */
        if(buffer[i]=='\n')
        {
            line++;
            column=0;
        }

        tkn=&(IN->tokens[j]);
        index=&buffer[i];

        /* ¿ Is it a number ? */
        aux=isANumber(index,
        (char *)&((tkn->operand).txtReference));
        if(index!=aux)
        {
            (tkn->operand).column=column;
            (tkn->operand).line=line;
            (tkn->operand).dimension=0;
            
            if(strstr((tkn->operand).txtReference,".")!=NULL)
            {   // -2 for REALS
                (tkn->operand).type=-2;
                (tkn->operand).content.real=
                    atof((tkn->operand).txtReference);
            }
            else
            {   // -1 for INTEGERS
                (tkn->operand).type=-1;
                (tkn->operand).content.integer=
                    atoi((tkn->operand).txtReference);
            }
            
            i+=aux-index-1;
            j++;
            continue;
        }
        /* Is it a variable?  */
        // type=0 for all variables: abc, $abc, $, #
        aux=isAVariable(index,
            (char *)&((tkn->operand).txtReference));
        if(index!=aux)
        {
            (tkn->operand).type=0;
            (tkn->operand).line=line;
            (tkn->operand).column=column;
            i+=aux-index-1;
            j++;
            continue;
        } 
        /* Is it an Operator? */
        // type > 0 and it is assigned by operatorType
        aux=isAnOperator(index,
            (short*)&((tkn->operator).type));
        if(index!=aux)
        {
            (tkn->operator).column=column;
            (tkn->operator).line=line;
            i+=aux-index-1;
            j++;
            continue;
        }

        // "Error: Program too long, many symbols to process." 
        if(j>=maximum_Tokens)
            return 3;
    }

    (*IN).initial=0;
    (*IN).items=j;

    //-For each pair of operands-
    for(i=(*IN).initial+1;i<(*IN).items;i++)
    if((*IN).tokens[i].operand.type<=0 &&
    ((*IN).tokens[i-1].operand.type<=0 ||
     (*IN).tokens[i-1].operand.type>105))
    {//–DO-----------------------
        j=FALSE;

        if((*IN).tokens[i].operand.type==0 &&
        (*IN).tokens[i].operand.txtReference[0]=='-')
        {
            j=TRUE;
            for(k=0;(*IN).tokens[i].operand.
                    txtReference[k]!='\0';k++)
                (*IN).tokens[i].operand.txtReference[k]=
                (*IN).tokens[i].operand.txtReference[k+1];
        }

        if((*IN).tokens[i].operand.type==-1 &&
        (*IN).tokens[i].operand.content.integer<0)
        {
            (*IN).tokens[i].operand.content.integer=
            -(*IN).tokens[i].operand.content.integer;
            j=TRUE;
        }

        if((*IN).tokens[i].operand.type==-2 &&
        (*IN).tokens[i].operand.content.real<0)
        {
            (*IN).tokens[i].operand.content.real=
            -(*IN).tokens[i].operand.content.real;
            j=TRUE;
        }

        //"[%i,%i] Error: A ``,´´ was expected before ``%s´´.\n"
        if(!j)
        {
        printf("[%i,%i] Error: A ``,´´ was expected before ``%s´´.\n",
            (*IN).tokens[i].operand.line,
            (*IN).tokens[i].operand.column,
            (*IN).tokens[i].operand.txtReference);
            return 4;
        }
            

        // "Error: Program too long, many symbols to process." 
        if((*IN).items>=maximum_Tokens)
            return 3;

        for(k=(*IN).items-1; k>=i; k--)
            (*IN).tokens[k+1]=(*IN).tokens[k];
        (*IN).items++;

        (*IN).tokens[i].operator.type=42; //42==operatorType("-")
        // Same line
        (*IN).tokens[i].operator.line=(*IN).tokens[i+1].operand.line;
        // A char(column) backwards 
        (*IN).tokens[i].operator.column=(*IN).tokens[i+1].operand.column-1;

    }//----END OF FOR-----------



    printf("\n%u\n",j); 
    for(i=(*IN).initial;i<(*IN).items;i++)
        if((*IN).tokens[i].operator.type>0)
            printf("\t%u (%u): OPTR %u\n",i,
            (*IN).tokens[i].operator.line,
            (*IN).tokens[i].operator.type);
        else
            printf("\t%u (%u):%lli (%i)\n",i,
            (*IN).tokens[i].operand.line,
            (*IN).tokens[i].operand.content.integer, 
            (*IN).tokens[i].operand.type);


    return 0;
}