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
        //(*IN).tokens[i].operator.column=(*IN).tokens[i+1].operand.column;
        // So, the next Token begins in the following char
        //(*IN).tokens[i+1].operand.column=(*IN).tokens[i+1].operand.column+2;

    }//----END OF FOR-----------



    /* printf("\n%u\n",j); 
    for(i=(*IN).initial;i<(*IN).items;i++)
        if((*IN).tokens[i].operator.type>0)
            printf("\t%u (%u): OPTR %u\n",i,
            (*IN).tokens[i].operator.line,
            (*IN).tokens[i].operator.type);
        else
            printf("\t%u (%u):%lli (%i)\n",i,
            (*IN).tokens[i].operand.line,
            (*IN).tokens[i].operand.content.integer, 
            (*IN).tokens[i].operand.type);*/


    return 0;
}


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
                else if(k)
                {
                    printf("Error: Uncorresponding pairs ``´´.\n");
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

int toPostfix(TokenStack *IN, TokenStack *OUT)
{
    TokenStack AUX;
    Token tkn;
    int i,j;

    // Error for dissmaching [] () {}
    if(!verifyPairs(IN,&AUX))
        return 1;
    
    initTokenStack(&AUX);
    initTokenStack(OUT);
    while(!emptyTokenStack(IN))
    {  
        tkn=popTokenStack(IN);
        if(tkn.operand.type<=0)
            pushTokenStack(OUT,tkn);
        else
        {
            if(emptyTokenStack(&AUX))
            {
                pushTokenStack(&AUX,tkn);
                continue;
            } 
            // A closing symbol ] } )
            if(tkn.operator.type>105)
            {
                while(topTokenStack(&AUX).operator.type<100)
                    pushTokenStack(OUT,popTokenStack(&AUX));
                popTokenStack(&AUX);
                continue;
            }
            //An opening symbol ( [ {
            if(tkn.operator.type>100)
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

int fileToPrefix(char* file_name,TokenStack *ts)
{
    TokenStack aux;

    if(loadFile(file_name,&aux))
        return FALSE;
    if(toPostfix(&aux,ts))
        return FALSE;

    return TRUE;
}
