#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define error_exit( str ) do { \
    fprintf (stderr, "ERROR : " #str "\n" ); exit (1); \
} while (0)

struct token *arr[1000];
int end = 0;
int paren_cnt = 0;

typedef enum {
    NUMBER,
    PAR_OPEN,
    PAR_CLOSE,
    PLUS,
    MINUS,
    ASTERISK,
    SLASH,
    PERCENT,
    CARET
} token_t;

struct token {
    double value;
    token_t type;
};

void add_token (double value, token_t type) {
    struct token *ptr = malloc (sizeof (struct token));
    ptr->value = value;
    ptr->type = type;
    arr[end++] = ptr;
}

void tokenize (char *str) {
    char buf[20] = {}; int j;
    for (int i = 0; i < strlen(str); i++) 
    {
        switch (str[i]) 
        {
            case '(' : puts("paran-open"); add_token ('(', PAR_OPEN); break;
            case ')' : puts("paran-close"); add_token (')', PAR_CLOSE); break;
            case '+' : puts("plus"); add_token ('+', PLUS); break;
            case '-' : puts("minus"); add_token ('-', MINUS); break;
            case '*' : puts("asterisk"); add_token ('*', ASTERISK); break;
            case '/' : puts("slash"); add_token ('/', SLASH); break;
            case '%' : puts("percent"); add_token ('%', PERCENT); break;
            case '^' : puts("caret"); add_token ('^', CARET); break;
            case '1' : case '2' : case '3' : case '4' : case '5' :
            case '6' : case '7' : case '8' : case '9' : case '0' : case '.' :
                       puts("number"); j = 0; 
                       while (isdigit (str[i]) || str[i] == '.') buf[j++] = str[i++];
                       buf[j] = '\0'; i -= 1;
                       add_token (atof(buf), NUMBER); 
                       break;
            default  : puts("blank");
        }
    }
    if (end == 0) error_exit("No available tokens exist");

    printf ("total tokens : %d\n", end);
    for (int i = 0; i < end; i++) {
        if (arr[i]->type == NUMBER) 
            printf ("value : %g\n", arr[i]->value);
        else
            printf ("value : %c\n", (char) arr[i]->value);
    }
}

#define BINARY_EVAL( $name, $op ) \
double $name (double left, double right) \
{ \
    printf (#$name "() left : %g, right : %g\n", left, right); \
    return left $op right; \
}

BINARY_EVAL (eval_add, +);
BINARY_EVAL (eval_sub, -);
BINARY_EVAL (eval_mul, *);
BINARY_EVAL (eval_div, /);

double eval_pow (double left, double right) {
    printf ("eval_pow() left : %g, right : %g\n", left, right);
    return pow (left, right);
}
double eval_mod (double left, double right) {
    printf ("eval_mod() left : %g, right : %g\n", left, right);
    return fmod (left, right);
}
double eval_plus (double val) {
    printf ("eval_plus() value : %g\n", val);
    return val;
}
double eval_minus (double val) {
    printf ("eval_minus() value : %g\n", val);
    return - val;
}

int parse_expr (int begin, double *ret);

int parse_primary_expr (int begin, double *ret)
{
    puts ("parse_primary_expr()");
    if (begin >= end) exit(1);

    int token_cnt = 0;
    if (arr[begin]->type == NUMBER) {
        *ret = arr[begin]->value;
        printf ("NUMBER : %g\n", *ret);
        return 1;
    }
    if (arr[begin]->type == PAR_OPEN) {
        puts (" (  PAR_OPEN"); paren_cnt++;
        token_cnt++;
    } else 
        return -1;

    token_cnt += parse_expr (begin + token_cnt, ret);

    if (begin + token_cnt++ >= end)
        error_exit ("Parentheses missmatch");
    puts (" )  PAR_CLOSE"); paren_cnt--;

    return token_cnt;
}

int parse_factor (int begin, double *ret)
{
    puts ("parse_factor()");
    int token_cnt = 0;
    double value;
    token_t type;
    int tmp = parse_primary_expr (begin, &value);
    if (tmp >= 0) { 
        if (begin + tmp < end && arr[begin + tmp]->type == CARET) {
            int token_cnt = tmp;
            double right;
            token_cnt++;
            token_cnt += parse_factor (begin + token_cnt, &right);
            *ret = eval_pow (value, right);
            return token_cnt;
        }
        *ret = value; 
        return tmp; 
    }
    if (arr[begin]->type != PLUS && arr[begin]->type != MINUS)
        error_exit ("Only unary PLUS or MINUS allowed");

    token_cnt++;
    token_cnt += parse_factor (begin + 1, &value);
    switch (arr[begin]->type) {
        case PLUS :
            *ret = eval_plus (value);
            break;
        case MINUS :
            *ret = eval_minus (value);
        default : ;
    }
    return token_cnt;
}

int parse_term (int begin, double *ret)
{
    puts ("parse_term()");
    int token_cnt = 0;
    double left, right;
    token_cnt += parse_factor (begin, &left);
    if (begin + token_cnt < end) {
        if (arr[begin + token_cnt]->type == NUMBER)
            error_exit ("Consecutive NUMBER");
        if (arr[begin + token_cnt]->type == PAR_OPEN)
            error_exit ("Missing operator before PAR_OPEN ?");
    }
    while ( begin + token_cnt < end 
            && (arr[begin + token_cnt]->type == ASTERISK 
                || arr[begin + token_cnt]->type == SLASH
                || arr[begin + token_cnt]->type == PERCENT ))
    {
        token_t type = arr[begin + token_cnt]->type;
        token_cnt++;
        token_cnt += parse_factor (begin + token_cnt, &right);
        switch (type) {
            case ASTERISK : 
                left = eval_mul (left, right);
                break;
            case SLASH : 
                left = eval_div (left, right);
                break;
            case PERCENT : 
                left = eval_mod (left, right);
            default : ;
        }
    }
    *ret = left;
    return token_cnt;
}

int parse_expr (int begin, double *ret)
{
    puts ("parse_expr()");
    int token_cnt = 0;
    double left, right;
    token_cnt += parse_term (begin, &left);
    while ( begin + token_cnt < end 
            && (arr[begin + token_cnt]->type == PLUS 
                || arr[begin + token_cnt]->type == MINUS ))
    {
        token_t type = arr[begin + token_cnt]->type;
        token_cnt++;
        token_cnt += parse_term (begin + token_cnt, &right);
        switch (type) {
            case PLUS : 
                left = eval_add (left, right);
                break;
            case MINUS : 
                left = eval_sub (left, right);
            default : ;
        }
    }
    if (begin + token_cnt < end && arr[begin + token_cnt]->type == PAR_CLOSE 
        && paren_cnt == 0) error_exit ("Parentheses missmatch");

    *ret = left;
    return token_cnt;
}

int main (int argc, char *argv[]) 
{
    if (argc == 1) return 1;

    puts ("==========  tokenize()  =========");
    tokenize (argv[1]);
    puts ("===========  parse()  ===========");
    double result;
    parse_expr (0, &result);
    puts ("============  result  ===========");
    printf ("result : %g\n", result);

    return 0;
}
