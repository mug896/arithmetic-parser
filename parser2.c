/*
 *   sh$ gcc parser2.c -lm
 *   sh$ ./a.out '1 + 2 * 3'
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

void error_exit(char *msg)
{ 
    fprintf(stderr, "ERROR: %s\n", msg); 
    exit(EXIT_FAILURE);
}

struct token **toks;
int end = 0;
int paren_cnt = 0;

enum token_type {
    NUMBER,
    LPAREN,
    RPAREN,
    PLUS,
    MINUS,
    ASTERISK,
    SLASH,
    PERCENT,
    CARET
};

struct token {
    double value;
    enum token_type type;
};

void add_token(double value, enum token_type type) 
{
    struct token *ptr = malloc(sizeof(struct token));
    ptr->value = value;
    ptr->type = type;
    toks[end++] = ptr;
}

void tokenize(char *str)
{
    char buf[20] = {}; int j;
    for (int i = 0; i < strlen(str); i++) 
    {
        switch (str[i]) 
        {
            case '(' : puts("paran-open");  add_token('(', LPAREN);   break;
            case ')' : puts("paran-close"); add_token(')', RPAREN);   break;
            case '+' : puts("plus");        add_token('+', PLUS);     break;
            case '-' : puts("minus");       add_token('-', MINUS);    break;
            case '*' : puts("asterisk");    add_token('*', ASTERISK); break;
            case '/' : puts("slash");       add_token('/', SLASH);    break;
            case '%' : puts("percent");     add_token('%', PERCENT);  break;
            case '^' : puts("caret");       add_token('^', CARET);    break;
            case '1' : case '2' : case '3' : case '4' : case '5' :
            case '6' : case '7' : case '8' : case '9' : case '0' : case '.' :
                       puts("number"); j = 0; 
                       while (isdigit (str[i]) || str[i] == '.')
                           buf[j++] = str[i++];
                       buf[j] = '\0'; i -= 1;
                       add_token(atof(buf), NUMBER); 
                       break;
            default  : puts("blank");
        }
    }
    if (end == 0) error_exit("No available tokens exist");

    printf("total tokens : %d\n", end);
    for (int i = 0; i < end; i++) {
        if (toks[i]->type == NUMBER) 
            printf("value : %.10g\n", toks[i]->value);
        else
            printf("value : %c\n", (char) toks[i]->value);
    }
}

#define BINARY_EVAL( $name, $op ) \
double $name(double num1, double num2) \
{ \
    printf(#$name "() num1 : %.10g, num2 : %.10g\n", num1, num2); \
    return num1 $op num2; \
}

BINARY_EVAL(eval_add, +);
BINARY_EVAL(eval_sub, -);
BINARY_EVAL(eval_mul, *);
BINARY_EVAL(eval_div, /);

double eval_pow(double num1, double num2) {
    printf("eval_pow() num1 : %.10g, num2 : %.10g\n", num1, num2);
    return pow(num1, num2);
}
double eval_mod(double num1, double num2) {
    printf("eval_mod() num1 : %.10g, num2 : %.10g\n", num1, num2);
    return fmod(num1, num2);
}
double eval_plus(double num1) {
    printf("eval_plus() num1 : %.10g\n", num1);
    return num1;
}
double eval_minus(double num1) {
    printf("eval_minus() num1 : %.10g\n", num1);
    return - num1;
}

int parse_expr(int begin, double *ret);

int parse_primary_expr(int begin, double *ret)
{
    puts("parse_primary_expr()");
    if (begin >= end)
        error_exit("Early termination");

    int token_cnt = 0;
    if (toks[begin]->type == NUMBER) {
        *ret = toks[begin]->value;
        printf("NUMBER : %.10g\n", *ret);
        return 1;
    }
    if (toks[begin]->type == LPAREN) {
        puts(" (  LPAREN"); paren_cnt++;
        token_cnt++;
    } else 
        return -1;

    token_cnt += parse_expr(begin + token_cnt, ret);

    if (begin + token_cnt++ >= end)
        error_exit("Parentheses missmatch");
    puts(" )  RPAREN"); paren_cnt--;

    return token_cnt;
}

int parse_factor(int begin, double *ret)
{
    puts("parse_factor()");
    int token_cnt = 0;
    double num1;
    enum token_type type;

    int tmp = parse_primary_expr(begin, &num1);
    if (tmp >= 0) { 
        if (begin + tmp < end && toks[begin + tmp]->type == CARET) {
            int token_cnt = tmp;
            double num2;
            token_cnt++;
            token_cnt += parse_factor(begin + token_cnt, &num2);
            *ret = eval_pow(num1, num2);
            return token_cnt;
        }
        *ret = num1; 
        return tmp; 
    }
    if (toks[begin]->type != PLUS && toks[begin]->type != MINUS)
        error_exit("Only unary PLUS or MINUS allowed");

    token_cnt++;
    token_cnt += parse_factor(begin + 1, &num1);
    switch (toks[begin]->type) {
        case PLUS :
            *ret = eval_plus(num1);
            break;
        case MINUS :
            *ret = eval_minus(num1);
        default : ;
    }
    return token_cnt;
}

int parse_term(int begin, double *ret)
{
    puts ("parse_term()");
    int token_cnt = 0;
    double num1, num2;

    token_cnt += parse_factor(begin, &num1);

    if (begin + token_cnt < end) {
        if (toks[begin + token_cnt]->type == NUMBER)
            error_exit("Consecutive NUMBER");
        if (toks[begin + token_cnt]->type == LPAREN)
            error_exit("Missing operator before LPAREN ?");
    }
    while ( begin + token_cnt < end 
            && (toks[begin + token_cnt]->type == ASTERISK 
                || toks[begin + token_cnt]->type == SLASH
                || toks[begin + token_cnt]->type == PERCENT ))
    {
        enum token_type type = toks[begin + token_cnt]->type;
        token_cnt++;
        token_cnt += parse_factor(begin + token_cnt, &num2);
        switch (type) {
            case ASTERISK : 
                num1 = eval_mul(num1, num2);
                break;
            case SLASH : 
                num1 = eval_div(num1, num2);
                break;
            case PERCENT : 
                num1 = eval_mod(num1, num2);
            default : ;
        }
    }
    *ret = num1;
    return token_cnt;
}

int parse_expr(int begin, double *ret)
{
    puts("parse_expr()");
    int token_cnt = 0;
    double num1, num2;

    token_cnt += parse_term(begin, &num1);

    while ( begin + token_cnt < end 
            && (toks[begin + token_cnt]->type == PLUS 
                || toks[begin + token_cnt]->type == MINUS ))
    {
        enum token_type type = toks[begin + token_cnt]->type;
        token_cnt++;
        token_cnt += parse_term(begin + token_cnt, &num2);
        switch (type) {
            case PLUS : 
                num1 = eval_add(num1, num2);
                break;
            case MINUS : 
                num1 = eval_sub(num1, num2);
            default : ;
        }
    }
    if (begin + token_cnt < end && toks[begin + token_cnt]->type == RPAREN 
        && paren_cnt == 0) error_exit("Parentheses missmatch");

    *ret = num1;
    return token_cnt;
}

int main(int argc, char *argv[]) 
{
    if (argc < 2)
        error_exit("Arithmetic expression required");

    puts("==========  tokenize()  =========");
    toks = malloc(sizeof(void *) * strlen(argv[1]));
    tokenize(argv[1]);
    puts("===========  parse()  ===========");
    double result;
    parse_expr(0, &result);
    puts("============  result  ===========");
    printf("result : %.10g\n", result);
    for (int i = 0; i < end; i++)
        free(toks[i]);
    free(toks);
    return 0;
}
