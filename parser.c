/*
 *   sh$ gcc parser.c -lm
 *   sh$ ./a.out '1 + 2 * 3'
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

void error_exit(char *msg)
{ 
    fprintf(stderr, "Error: %s\n", msg); 
    exit(EXIT_FAILURE);
}

struct token **toks;
int end = 0; 
int paren_cnt = 0;

enum token_type {
    NUMBER,
    RPAREN,
    LPAREN,
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

typedef struct node {
    double (*eval)(struct node *);
} node;

typedef struct literal_node {      // 구조체의 멤버들은 순서가 유지된다는 것을 이용합니다.
    double (*eval)(node *);        // 따라서 eval 함수에 해당하는 함수 포인터를 제일 위에 두면
    double value;                  // node *foo 타입의 포인터에서도 실행할 수 있게됩니다.
} literal_node;

typedef struct unary_node {
    double (*eval)(node *);
    struct node *num1;
} unary_node;

typedef struct binary_node {
    double (*eval)(node *);
    struct node *num1;
    struct node *num2;
} binary_node;

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

#define BINARY_EVAL( $name, $op ) \
double $name(node *this) \
{ \
    node *num1  = ((binary_node *) this)->num1; \
    node *num2 = ((binary_node *) this)->num2; \
    double res = num1->eval(num1) $op num2->eval(num2); \
    puts("free binary_node " #$name); free(this); \
    return res; \
}

BINARY_EVAL(eval_add, +);
BINARY_EVAL(eval_sub, -);
BINARY_EVAL(eval_mul, *);
BINARY_EVAL(eval_div, /);

double eval_pow(node *this)
{
    node *num1 = ((binary_node *) this)->num1;
    node *num2 = ((binary_node *) this)->num2;
    double res = pow(num1->eval(num1), num2->eval(num2));
    puts("free binary_node eval_pow"); free(this);
    return res;
}

double eval_mod(node *this)
{
    node *num1 = ((binary_node *) this)->num1;
    node *num2 = ((binary_node *) this)->num2;
    double res = fmod(num1->eval(num1), num2->eval(num2));
    puts("free binary_node eval_mod"); free(this);
    return res;
}

double eval_plus(node *this)
{ 
    node *num1 = ((unary_node *) this)->num1;
    double res = num1->eval(num1);
    puts("free unary_node eval_plus"); free(this);
    return res;
}

double eval_minus(node *this)
{ 
    node *num1 = ((unary_node *) this)->num1;
    double res = - num1->eval(num1);
    puts("free unary_node eval_minus"); free(this);
    return res;
}

double eval_literal(node *this)
{
    double res = ((literal_node *) this)->value;
    printf("free literal_node : %.10g\n", res); free(this);
    return res;
}

#define MAKE_LITERAL_NODE( $value, $ast ) do { \
    literal_node *p = malloc(sizeof(literal_node)); \
    p->value = $value; \
    p->eval = eval_literal; \
    $ast = (node *) p; \
    printf("make_literal_node : %.10g\n", $value); \
} while (0)

#define MAKE_UNARY_NODE( $num1, $eval, $ast ) do { \
    unary_node *p = malloc(sizeof(unary_node)); \
    p->num1 = $num1; \
    p->eval = $eval; \
    $ast = (node *) p; \
    puts("make_unary_node " #$eval); \
} while (0)

#define MAKE_BINARY_NODE( $num1, $num2, $eval, $ast ) do { \
    binary_node *p = malloc(sizeof(binary_node)); \
    p->num1 = $num1; \
    p->num2 = $num2; \
    p->eval = $eval; \
    $ast = (node *) p; \
    puts("make_binary_node " #$eval); \
} while (0)

/*
 *   이후부터는 연산자 우선순위별로 함수를 작성하면 됩니다.
 *   "^" 거듭제곱 연산자는 오른쪽부터 계산하는 right associativity 로써
 *   곱셈, 나눗셈 보다 먼저 계산돼야 하므로 parse_factor() 에 포함시킵니다.
 *
 *   <factor> ::= <primary-expr> 
 *                | <primary-expr> ^ <factor> 
 *                | "+" <factor> 
 *                | "-" <factor>
 *
 *  abstract syntax tree 는 main 함수의 node *ast 포인터 변수에 만들어지는데,
 *  예를 들어 parse_factor() 함수에서 설정한 node *num1 를 parse_primary_expr() 함수의 **ast 인수로 전달하면
 *  parse_primary_expr() 함수에서는 MAKE_LITERAL_NODE 할때 *ast 를 사용하면 num1 포인터 변수가 설정되겠죠.
 *  parse_factor() 함수에서는 다시 num1 포인터를 *ast 에 설정한후 return 하고 ... 하면
 *  recursive 하게 abstract syntax tree 가 최종적으로 node *ast 변수에 만들어지게 됩니다.
 *  recursion 을 이용한 코드는 처음에는 이해하기가 어려우므로 시간을 가지고 분석해야 합니다.
 */

int parse_expr(int begin, node **ast);

int parse_primary_expr(int begin, node **ast)
{
    puts("RUN parse_primary_expr()");
    if (begin >= end)
        error_exit("Early termination");

    int token_cnt = 0;
    if (toks[begin]->type == NUMBER) {
        MAKE_LITERAL_NODE(toks[begin]->value, *ast);
        return 1;
    }
    if (toks[begin]->type == LPAREN) {
        puts(" (  LPAREN"); paren_cnt++;
        token_cnt++;
    } 
    else return -1;

    token_cnt += parse_expr(begin + token_cnt, ast);

    if (begin + token_cnt++ >= end)
        error_exit("Parentheses missmatch");
    puts(" )  RPAREN"); paren_cnt--;

    return token_cnt;
}

int parse_factor(int begin, node **ast)
{
    puts("RUN parse_factor()");
    int token_cnt = 0;
    node *num1;

    int tmp = parse_primary_expr(begin, &num1);
    if (tmp >= 0) { 
        if (begin + tmp < end && toks[begin + tmp]->type == CARET) {      // "^" 연산자는 오른쪽부터 계산하는 
            int token_cnt = tmp;                                          // right associativity 로써 트리를
            node *num2;                                                   // 오른쪽에 만들어 나가야합니다.
            token_cnt++;                                                  // 따라서 while 문을 이용해 트리를 만들지 않고
            token_cnt += parse_factor(begin + token_cnt, &num2);          // 자기 자신을 재귀적으로 호출한후에
            MAKE_BINARY_NODE(num1, num2, eval_pow, *ast);                 // return 하면서 트리를 만들어 나갑니다.
            return token_cnt;                                             //
        }                                                                 //   4 ^ 3 ^ 2  일경우         ^
        *ast = num1;                                                      //                         4     ^
        return tmp;                                                       //                            3     2
    } 
    if (toks[begin]->type != PLUS && toks[begin]->type != MINUS)
        error_exit("Only unary PLUS or MINUS allowed");

    token_cnt++;
    token_cnt += parse_factor(begin + 1, &num1);           // "+" <factor> 와 "-" <factor> 를 만드는
    switch (toks[begin]->type) {                           // MAKE_UNARY_NODE 의 경우도 오른쪽 <factor> 가
        case PLUS :                                        // 먼저 계산이 완료돼야 하므로 자기 자신을 재귀적으로 
            MAKE_UNARY_NODE(num1, eval_plus, *ast);        // 호출한후에 return 하면서 트리를 만듭니다.
            break;
        case MINUS :
            MAKE_UNARY_NODE(num1, eval_minus, *ast);
        default : ;
    }
    return token_cnt;
}

int parse_term(int begin, node **ast)
{
    puts("RUN parse_term()");
    int token_cnt = 0;
    node *num1, *num2;

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
        token_cnt += parse_factor(begin + token_cnt, &num2);         // 곱셉, 나눗셈은 왼쪽부터 계산하는
        switch (type) {                                              // left associativity 이므로 parse_factor() 
            case ASTERISK :                                          // 함수를 이용해 node *num2 를 설정해서
                MAKE_BINARY_NODE(num1, num2, eval_mul, num1);        // while 문을 이용해 왼쪽에 트리를 만들어 나갑니다.
                break;                                               // 덧셈, 뺼셈과 곱셈, 나눗셈의 관계는 
            case SLASH :                                             // 곱셈, 나눗셈이 우선순위가 높기 때문에 
                MAKE_BINARY_NODE(num1, num2, eval_div, num1);        // parse_term() 함수를 별도로 만들어야 합니다.
                break;
            case PERCENT :
                MAKE_BINARY_NODE(num1, num2, eval_mod, num1);
            default : ;
        }
    }
    *ast = num1;
    return token_cnt;
}

int parse_expr(int begin, node **ast)
{
    puts("RUN parse_expr()");
    int token_cnt = 0;
    node *num1, *num2;

    token_cnt += parse_term(begin, &num1);

    while ( begin + token_cnt < end 
            && (toks[begin + token_cnt]->type == PLUS 
                || toks[begin + token_cnt]->type == MINUS ))
    {
        enum token_type type = toks[begin + token_cnt]->type;
        token_cnt++;
        token_cnt += parse_term(begin + token_cnt, &num2);           // 덧셈, 뺄셈도 왼쪽부터 계산하는
        switch (type) {                                              // left associativity 이므로 parse_term() 
            case PLUS :                                              // 함수를 이용해 node *num2 를 설정해서
                MAKE_BINARY_NODE(num1, num2, eval_add, num1);        // while 문을 이용해 왼쪽에 트리를 만들어 나갑니다.
                break;                                               //
            case MINUS :                                             //   2 - 3 + 4  일경우           +
                MAKE_BINARY_NODE(num1, num2, eval_sub, num1);       //                           -     4
            default : ;                                              //                        2     3
        }
    }
    if (begin + token_cnt < end && toks[begin + token_cnt]->type == RPAREN 
        && paren_cnt == 0) error_exit("Parentheses missmatch");

    *ast = num1;
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
    node *ast;
    parse_expr(0, &ast);
    puts("============  eval()  ===========");
    printf("result : %.10g\n", ast->eval(ast));
    for (int i = 0; i < end; i++)
        free(toks[i]);
    free(toks);
    return 0;
}
