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
    fprintf(stderr, "\nError: %s\n", msg); 
    exit(EXIT_FAILURE);
}

struct token **toks;
int end = 0;
int token_cnt = 0;
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
            case ' ' : puts("blank");
                       break;
            default :; 
                    char msg[20];
                    sprintf(msg, "tokenizer: \"%c\"", str[i]);
                    error_exit(msg);
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

////////////////////////////////////////////////////////////

void error(int addend) 
{
    fputs("\nError: ", stderr);
    for (int i = 0; i < token_cnt + addend; i++) {
        switch (toks[i]->type) {
            case NUMBER :
                fprintf(stderr, "%.10g ", toks[i]->value);
                break;
            default:
                fprintf(stderr, "%c ", (int) toks[i]->value);
        }
    }
    if (token_cnt == end && paren_cnt > 0)
        fputs("  <--- \")\"\n", stderr);
    else
        fputs("  <---\n", stderr);
    exit(1);
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

double parse_expr();

double parse_primary_expr()
{
    double num1;
    puts("parse_primary_expr()");
    enum token_type type = toks[token_cnt]->type;
    if (type == NUMBER) {
        num1 = toks[token_cnt]->value;
        printf("NUMBER : %.10g\n", num1);
    }
    else if (type == LPAREN) {
        puts(" (  LPAREN"); paren_cnt++;
        token_cnt++;

        num1 = parse_expr();

        token_cnt++;
        if (! (token_cnt < end)) error(0);  // 오류: 1 + ( 2
        puts(" )  RPAREN"); paren_cnt--;
    } 
    else error(1);   // 오류: 1 + 2 / *

    return num1;
}

double parse_factor()
{
    puts("parse_factor()");
    double num1;
    if (! (token_cnt < end)) error(0);  // 오류: 1 + 2 +
    enum token_type type = toks[token_cnt]->type;
    if (type != PLUS && type != MINUS)
        num1 = parse_primary_expr();

    if (type == PLUS || type == MINUS) {
        token_cnt++;
        num1 = parse_factor();
        switch (type) {
            case PLUS :
                num1 = eval_plus(num1);
                break;
            case MINUS :
                num1 = eval_minus(num1);
            default :;
        }
    }
    return num1;
}

double parse_power()
{
    puts ("parse_power()");
    double num1, num2;

    num1 = parse_factor();
    if (token_cnt + 1 < end && toks[token_cnt + 1]->type == CARET) 
    {
        token_cnt++; token_cnt++;
        double num2 = parse_power();
        num1 = eval_pow(num1, num2);
    }
    return num1;
}

double parse_term()
{
    puts ("parse_term()");
    double num1, num2;

    num1 = parse_power();
    while ( token_cnt + 1 < end &&
                (  toks[token_cnt + 1]->type == ASTERISK 
                || toks[token_cnt + 1]->type == SLASH
                || toks[token_cnt + 1]->type == PERCENT ))
    {
        token_cnt++;
        enum token_type type = toks[token_cnt]->type;
        token_cnt++;
        num2 = parse_factor();
        switch (type) {
            case ASTERISK : 
                num1 = eval_mul(num1, num2);
                break;
            case SLASH : 
                num1 = eval_div(num1, num2);
                break;
            case PERCENT : 
                num1 = eval_mod(num1, num2);
            default :;
        }
    }
    return num1;
}

double parse_expr()
{
    puts("parse_expr()");
    double num1, num2;

    num1 = parse_term();

    while (token_cnt + 1 < end &&
                (  toks[token_cnt + 1]->type == PLUS 
                || toks[token_cnt + 1]->type == MINUS ))
    {
        token_cnt++;
        enum token_type type = toks[token_cnt]->type;
        token_cnt++;
        num2 = parse_term();
        switch (type) {
            case PLUS : 
                num1 = eval_add(num1, num2);
                break;
            case MINUS : 
                num1 = eval_sub(num1, num2);
            default :;
        }
    }
    if (token_cnt + 1 < end) {   // 오류처리
        switch (toks[token_cnt + 1]->type) {
            case LPAREN : error(2); break;   // 1 + 2 (3)
            case RPAREN : if (paren_cnt == 0) error(2); break;   // 2 ) + 3
            case NUMBER : error(2); break;   // 1 + 2 3
            default:;
        }
    }
    return num1;
}

int main(int argc, char *argv[]) 
{
    if (argc < 2)
        error_exit("Arithmetic expression required");

    puts("==========  tokenize()  =========");
    toks = malloc(sizeof(void *) * strlen(argv[1]));
    tokenize(argv[1]);
    puts("===========  parse()  ===========");
    double result = parse_expr();
    puts("============  result  ===========");
    printf("result : %.10g\n", result);
    for (int i = 0; i < end; i++)
        free(toks[i]);
    free(toks);
    return 0;
}
