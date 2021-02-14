
# Lexical Analyzer



![](lexer.png)


```c
const char lex_table[4][2] = {
    2, 1,
    3, 0,
    2, 3,
    3, 0
};

token_t *next_token () 
{
    static int cpos = 0;
    double value;
    enum token_type type;
    char buf[20] = {}; 

next_token :
    if (cpos >= strlen (input_str))
        return NULL;
    int i, j;
    int cur_stat = 0; 
    switch (input_str[cpos]) 
    {
        case '(' : value = '('; type = LPAREN; break;
        case ')' : value = ')'; type = RPAREN; break;
        case '+' : value = '+'; type = PLUS; break;
        case '-' : value = '-'; type = MINUS; break;
        case '*' : value = '*'; type = ASTERISK; break;
        case '/' : value = '/'; type = SLASH; break;
        case '%' : value = '%'; type = PERCENT; break;
        case '^' : value = '^'; type = CARET; break;
        case '1' : case '2' : case '3' : case '4' : case '5' :
        case '6' : case '7' : case '8' : case '9' : case '0' : case '.' :
                   i = cpos, j = 0; 
next_digit:                   
                   if (isdigit (input_str[i])) {
                       cur_stat = lex_table[cur_stat][0];
                   } else if (input_str[i] == '.') {
                       cur_stat = lex_table[cur_stat][1];
                   } else {
                        if (cur_stat == 2 || cur_stat == 3) {
                            buf[j] = '\0'; cpos = i - 1;
                            value = atof(buf); type = NUMBER;
                            break;
                        }
                        error_exit ("not a number");
                   }
                   if (cur_stat == 0)
                       error_exit ("not a number");
                   buf[j++] = input_str[i++]; 
                   goto next_digit;

        default  : cpos += 1; goto next_token;
    }
    cpos += 1;
    struct token *p = malloc (sizeof (struct token));
    p->value = value;
    p->type = type;
    return p;
}
```
