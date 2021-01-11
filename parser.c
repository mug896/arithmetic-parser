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

typedef struct node {
    double (*eval)(struct node *);
} node;

typedef struct literal_node {
    double (*eval)(node *);
    double value;
} literal_node;

typedef struct unary_node {
    double (*eval)(node *);
    struct node *child;
} unary_node;

typedef struct binary_node {
    double (*eval)(node *);
    struct node *left;
    struct node *right;
} binary_node;

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
    printf ("total tokens : %d\n", end);
    for (int i = 0; i < end; i++) {
        if (arr[i]->type == NUMBER) 
            printf ("value : %g\n", arr[i]->value);
        else
            printf ("value : %c\n", (char) arr[i]->value);
    }
}

#define BINARY_EVAL( $name, $op ) \
double $name (node *this) \
{ \
    node *left  = ((binary_node *) this)->left; \
    node *right = ((binary_node *) this)->right; \
    double res = left->eval(left) $op right->eval(right); \
    puts ("free binary_node " #$name); free(this); \
    return res; \
}

BINARY_EVAL (eval_add, +);
BINARY_EVAL (eval_sub, -);
BINARY_EVAL (eval_mul, *);
BINARY_EVAL (eval_div, /);

double eval_pow (node *this)
{
    node *left  = ((binary_node *) this)->left;
    node *right = ((binary_node *) this)->right;
    double lval = left->eval(left);
    double rval = right->eval(right);
    double res = pow (lval, rval);
    puts ("free binary_node eval_pow"); free(this);
    return res;
}

double eval_mod (node *this)
{
    node *left  = ((binary_node *) this)->left;
    node *right = ((binary_node *) this)->right;
    double res = fmod (left->eval(left), right->eval(right));
    puts ("free binary_node eval_mod"); free(this);
    return res;
}

double eval_plus (node *this)
{ 
    node *child = ((unary_node *) this)->child;
    double res = child->eval (child);
    puts ("free unary_node eval_plus"); free(this);
    return res;
}

double eval_minus (node *this)
{ 
    node *child = ((unary_node *) this)->child;
    double res = - child->eval (child);
    puts ("free unary_node eval_minus"); free(this);
    return res;
}

double eval_literal (node *this)
{
    double res = ((literal_node *) this)->value;
    puts ("free literal_node"); free(this);
    return res;
}

#define MAKE_LITERAL_NODE( $value, $ast ) do { \
    literal_node *p = malloc (sizeof (literal_node)); \
    p->value = $value; \
    p->eval = eval_literal; \
    $ast = (node *) p; \
    printf ("make_literal_node : %g\n", $value); \
} while (0)

#define MAKE_UNARY_NODE( $child, $eval, $ast ) do { \
    unary_node *p = malloc (sizeof (unary_node)); \
    p->child = $child; \
    p->eval = $eval; \
    $ast = (node *) p; \
    puts ("make_unary_node " #$eval); \
} while (0)

#define MAKE_BINARY_NODE( $left, $right, $eval, $ast ) do { \
    binary_node *p = malloc (sizeof (binary_node)); \
    p->left = $left; \
    p->right = $right; \
    p->eval = $eval; \
    $ast = (node *) p; \
    puts ("make_binary_node " #$eval); \
} while (0)

/*
 *   이후부터는 우선순위별로 함수를 작성하면 됩니다.
 *   "^" 거듭제곱 연산자는 오른쪽부터 계산하는 right associativity 로써
 *   곱셈, 나눗셈 보다 먼저 계산돼야 하므로 parse_term() 보다 앞에 parse_caret() 을 작성합니다.
 */

int parse_expr (int begin, node **ast);

int parse_primary_expr (int begin, node **ast)
{
    puts ("RUN parse_primary_expr()");
    int token_cnt = 0;
    if (arr[begin]->type == NUMBER) {
        MAKE_LITERAL_NODE (arr[begin]->value, *ast);
        return 1;
    }
    if (arr[begin]->type == PAR_OPEN) {
        puts (" (  PAR_OPEN"); paren_cnt++;
        token_cnt++;
    } else return -1;

    token_cnt += parse_expr (begin + token_cnt, ast);

    if (begin + token_cnt++ >= end)
        error_exit ("Parentheses missmatched. less PAR_CLOSE");
    puts (" )  PAR_CLOSE"); paren_cnt--;

    return token_cnt;
}

int parse_factor (int begin, node **ast)
{
    puts ("RUN parse_factor()");
    int token_cnt = 0;
    node *child;
    int tmp = parse_primary_expr (begin, &child);
    if (tmp >= 0) { *ast = child; return tmp; }

    token_cnt++;
    token_cnt += parse_factor (begin + 1, &child);
    if (arr[begin]->type == PLUS)
        MAKE_UNARY_NODE (child, eval_plus, *ast);
    else
        MAKE_UNARY_NODE (child, eval_minus, *ast);

    return token_cnt;
}

int parse_caret (int begin, node **ast)
{
    puts ("RUN parse_caret()");
    int token_cnt = 0;
    node *left, *right;
    token_cnt += parse_factor (begin, &left);
    if (begin + token_cnt < end && arr[begin + token_cnt]->type == CARET) {
        token_cnt++;
        token_cnt += parse_caret (begin + token_cnt, &right);
        MAKE_BINARY_NODE (left, right, eval_pow, left);
    }
    *ast = left;
    return token_cnt;
}

int parse_term (int begin, node **ast)
{
    puts ("RUN parse_term()");
    int token_cnt = 0;
    node *left, *right;
    token_cnt += parse_caret (begin, &left);
    while ( begin + token_cnt < end 
            && (arr[begin + token_cnt]->type == ASTERISK 
                || arr[begin + token_cnt]->type == SLASH
                || arr[begin + token_cnt]->type == PERCENT ))
    {
        int type = arr[begin + token_cnt]->type;
        token_cnt++;
        token_cnt += parse_caret (begin + token_cnt, &right);
        switch (type) {
            case ASTERISK : 
                MAKE_BINARY_NODE (left, right, eval_mul, left);
                break;
            case SLASH : 
                MAKE_BINARY_NODE (left, right, eval_div, left);
                break;
            case PERCENT : 
                MAKE_BINARY_NODE (left, right, eval_mod, left);
        }
    }
    *ast = left;
    return token_cnt;
}

int parse_expr (int begin, node **ast)
{
    puts ("RUN parse_expr()");
    int token_cnt = 0;
    node *left, *right;
    token_cnt += parse_term (begin, &left);
    while ( begin + token_cnt < end 
            && (arr[begin + token_cnt]->type == PLUS 
                || arr[begin + token_cnt]->type == MINUS ))
    {
        int type = arr[begin + token_cnt]->type;
        token_cnt++;
        token_cnt += parse_term (begin + token_cnt, &right);
        switch (type) {
            case PLUS : 
                MAKE_BINARY_NODE (left, right, eval_add, left);
                break;
            case MINUS : 
                MAKE_BINARY_NODE (left, right, eval_sub, left);
        }
    }
    if (begin + token_cnt < end && arr[begin + token_cnt]->type == PAR_CLOSE 
        && paren_cnt == 0) error_exit ("Parentheses missmatched. more PAR_CLOSE");

    *ast = left;
    return token_cnt;
}

int main (int argc, char *argv[]) 
{
    if (argc == 1) return 1;

    puts ("==========  tokenize()  =========");
    tokenize (argv[1]);
    node *ast;
    puts ("===========  parse()  ===========");
    parse_expr (0, &ast);
    puts ("============  eval()  ===========");
    printf ("result : %g\n", ast->eval (ast));

    return 0;
}
