/*
 *   sh$ gcc LR_parser.c -lm
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

const char *input_str;       // 입력 스트링 argv[1] 을 대입해 사용

typedef enum { 
    PLUS, MINUS, ASTERISK, SLASH, PERCENT, CARET, LPAREN,
    RPAREN, NUMBER, ENDMARK, EXPR, TERM, FACTOR, PEXPR
} token_t;

const char *const token_t_s[] = {
    "PLUS", "MINUS", "ASTERISK", "SLASH", "PERCENT", "CARET", "LPAREN",
    "RPAREN", "NUMBER", "ENDMARK", "EXPR", "TERM", "FACTOR", "PEXPR"
};

typedef struct token {       // 각각의 토큰을 struct token 구조체로 설정합니다.
    double value;
    token_t type;
    int state;
} token;

struct stack {               // stack 사용을 위한 구조체로 단방향 linked list 입니다.
    struct stack *prev;
    struct token *token;
};

struct stack *sp = NULL;     // stack 의 top 을 가리키는 포인터

void push(token *tok);
token *pop();
token *peek();

/*
 *  작성한 ACTION 과 GOTO 테이블을 그대로 행, 렬을 맞추어서 array 로 변환한 것입니다.
 *  양수 값은 s5, s4 같은 shift 를 나타내고 음수 값은 r3, r7 같은 reduce 를 나타냅니다.
 *  공백부분은 0 으로 채웠는데 파싱 도중에 여기를 가리키게 되면 오류가 됩니다.
 */
#define COLS 14
#define ROWS 25
const char table[ROWS][COLS] = 
{
      5,   4,   0,   0,   0,   0,   7,   0,   8,   0,   1,   2,   3,   6,
     10,   9,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
     -3,  -3,  11,  12,  13,   0,   0,  -3,   0,  -3,   0,   0,   0,   0,
     -7,  -7,  -7,  -7,  -7,   0,   0,  -7,   0,  -7,   0,   0,   0,   0,
      5,   4,   0,   0,   0,   0,   7,   0,   8,   0,   0,   0,  14,   6,
      5,   4,   0,   0,   0,   0,   7,   0,   8,   0,	0,   0,  15,   6,
    -11, -11, -11, -11, -11,  16,   0, -11,   0, -11,   0,   0,   0,   0,
      5,   4,   0,   0,   0,   0,   7,   0,   8,   0,  17,   2,   3,   6,
    -13, -13, -13, -13, -13, -13,   0, -13,   0, -13,   0,   0,   0,   0,
      5,   4,   0,   0,   0,   0,   7,   0,   8,   0,   0,  18,   3,   6,
      5,   4,   0,   0,   0,   0,   7,   0,   8,   0,   0,  19,   3,   6,
      5,   4,   0,   0,   0,   0,   7,   0,   8,   0,   0,   0,  20,   6,
      5,   4,   0,   0,   0,   0,   7,   0,   8,   0,   0,   0,  21,   6,
      5,   4,   0,   0,   0,   0,   7,   0,   8,   0,   0,   0,  22,   6,
     -8,  -8,  -8,  -8,  -8,   0,   0,  -8,   0,  -8,   0,   0,   0,   0,
     -9,  -9,  -9,  -9,  -9,   0,   0,  -9,   0,  -9,   0,   0,   0,   0,
      5,   4,   0,   0,   0,   0,   7,   0,   8,   0,   0,   0,  23,   6,
     10,   9,   0,   0,   0,   0,   0,  24,   0,   0,   0,   0,   0,   0,
     -1,  -1,  11,  12,  13,   0,   0,  -1,   0,  -1,   0,   0,   0,   0,
     -2,  -2,  11,  12,  13,   0,   0,  -2,   0,  -2,   0,   0,   0,   0,
     -4,  -4,  -4,  -4,  -4,   0,   0,  -4,   0,  -4,   0,   0,   0,   0,
     -5,  -5,  -5,  -5,  -5,   0,   0,  -5,   0,  -5,   0,   0,   0,   0,
     -6,  -6,  -6,  -6,  -6,   0,   0,  -6,   0,  -6,   0,   0,   0,   0,
    -10, -10, -10, -10, -10,   0,   0, -10,   0, -10,   0,   0,   0,   0,
    -12, -12, -12, -12, -12, -12,   0, -12,   0, -12,   0,   0,   0,   0
};

// 다음 토큰 하나를 전달하는 함수. 토큰을 하나씩 읽어들이면서 바로 파싱이 가능합니다.
token* next_token() 
{
    static int cpos = 0;
    double value;
    token_t type;
    char buf[20] = {}; 

next :
    if (cpos >= strlen(input_str))
        return NULL;
    int i, j;
    switch (input_str[cpos]) 
    {
        case '(' : value = '('; type = LPAREN;   break;
        case ')' : value = ')'; type = RPAREN;   break;
        case '+' : value = '+'; type = PLUS;     break;
        case '-' : value = '-'; type = MINUS;    break;
        case '*' : value = '*'; type = ASTERISK; break;
        case '/' : value = '/'; type = SLASH;    break;
        case '%' : value = '%'; type = PERCENT;  break;
        case '^' : value = '^'; type = CARET;    break;
        case '1' : case '2' : case '3' : case '4' : case '5' :
        case '6' : case '7' : case '8' : case '9' : case '0' : case '.' :
                   i = cpos, j = 0; 
                   while (isdigit(input_str[i]) || input_str[i] == '.')
                       buf[j++] = input_str[i++]; 
                   buf[j] = '\0'; cpos = i - 1;
                   value = atof(buf); type = NUMBER;
                   break;
        default  : cpos++; goto next;
    }
    cpos++;
    token *p = malloc(sizeof(token));
    p->value = value;
    p->type = type;
    return p;
}

void print_stack(struct stack *sp)    // 현재 stack 내용을 출력해 주는 함수
{
    if (sp->prev != NULL)
        print_stack(sp->prev);

    switch (sp->token->type) {
        case PLUS:     printf("[+]");  break;
        case MINUS:    printf("[-]");  break;
        case ASTERISK: printf("[*]");  break;
        case SLASH:    printf("[/]");  break;
        case PERCENT:  printf("[%%]"); break;
        case CARET:    printf("[^]");  break;
        case LPAREN:   printf("[(]");  break;
        case RPAREN:   printf("[)]");  break;
        case NUMBER:   printf("[%.10g]", sp->token->value); break;
        case ENDMARK:  printf("[$]");  break;
        case EXPR:     printf("[E]");  break;
        case TERM:     printf("[T]");  break;
        case FACTOR:   printf("[F]");  break;
        case PEXPR:    printf("[P]");  break;
    }
}

#define PRINT_STACK( $str ) \
    printf($str); print_stack(sp); puts("")

#define SET_STATE() tpeek = peek(); \
    tprev->state = table[ tpeek == NULL ? 0 : tpeek->state ][tprev->type]

#define REDUCE_BINARY( $op ) do { \
    tok2 = pop(); tok1 = pop(); tprev = pop(); \
    tprev->value = tprev->value $op tok2->value; \
    SET_STATE(); \
    push(tprev); free(tok2); free(tok1); \
} while (0)

#define REDUCE_MOD() do { \
    tok2 = pop(); tok1 = pop(); tprev = pop(); \
    tprev->value = fmod(tprev->value, tok2->value); \
    SET_STATE(); \
    push(tprev); free(tok2); free(tok1); \
} while (0)

#define REDUCE_CARET() do { \
    tprev = pop(); tok2 = pop(); tok1 = pop(); \
    tprev->value = pow(tok1->value, tprev->value); \
    SET_STATE(); \
    push(tprev); free(tok2); free(tok1); \
} while (0)

#define REDUCE_UNARY( $op ) do { \
    tprev = pop(); tok1 = pop(); \
    tprev->value = $op tprev->value; \
    SET_STATE(); \
    push(tprev); free(tok1); \
} while (0)

#define REDUCE_PAREN() do { \
    tok2 = pop(); tprev = pop(); tok1 = pop(); \
    tprev->type = PEXPR; \
    SET_STATE(); \
    push(tprev); free(tok2); free(tok1); \
} while (0)

#define REDUCE_TYPE( $type ) do { \
    tprev = pop(); \
    tprev->type = $type; \
    SET_STATE(); \
    push(tprev); \
} while (0)

double parse() 
{
    token_t type;
    int state;
    token *tnext, *tprev, *tpeek, *tok1, *tok2;

    if ((tnext = next_token()) == NULL) 
        error_exit("No available tokens exist");

    while (1) 
    {
        type = (tnext == NULL ? ENDMARK : tnext->type);
        tprev = peek();
        state = (tprev == NULL ? 0 : tprev->state);

        // state 가 1 이고 type 이 ENDMARK ( $ ) 이면 accept 가 되므로 결과를 반환하고 종료합니다.
        if (state == 1 && type == ENDMARK) {
            tok1 = pop();
            double res = tok1->value;
            free(tok1);
            return res;
        }

        // 테이블에서 state 와 type 의 교차지점 값이 0 이면 파싱 오류가 됩니다.
        if (table[state][type] == 0) {
            printf("\e[0;31mERROR:\e[0m (STATE: %d, TOKEN: %s) not allowed\n"
                    ,state, token_t_s[type]);
            exit(EXIT_FAILURE);
        }

        // 테이블에서 state 와 type 의 교차지점 값이 양수면 shift 가 되고 음수면 reduce 가 됩니다.
        if (table[state][type] > 0) {
            tnext->state = table[state][type];
            push(tnext);
            tnext = next_token(); 
            PRINT_STACK("\e[0;36mSHIFT:\e[0m ");
        } 
        else {
            switch (- table[state][type]) 
            {
                case  1: REDUCE_BINARY(-);    break;      // rule  1 : E -> E - T
                case  2: REDUCE_BINARY(+);    break;      // rule  2 : E -> E + T
                case  3: REDUCE_TYPE(EXPR);   break;      // rule  3 : E -> T
                case  4: REDUCE_BINARY(*);    break;      // rule  4 : T -> T * F
                case  5: REDUCE_BINARY(/);    break;      // rule  5 : T -> T / F
                case  6: REDUCE_MOD();        break;      // rule  6 : T -> T % F
                case  7: REDUCE_TYPE(TERM);   break;      // rule  7 : T -> F
                case  8: REDUCE_UNARY(-);     break;      // rule  8 : F -> - F
                case  9: REDUCE_UNARY(+);     break;      // rule  9 : F -> + F
                case 10: REDUCE_CARET();      break;      // rule 10 : F -> P ^ F
                case 11: REDUCE_TYPE(FACTOR); break;      // rule 11 : F -> P
                case 12: REDUCE_PAREN();      break;      // rule 12 : P -> ( E )
                case 13: REDUCE_TYPE(PEXPR);  break;      // rule 13 : P -> a
            }
            PRINT_STACK("\e[0;35mREDUC:\e[0m ");
        }
    }
}

int main(int argc, char *argv[]) 
{
    if (argc < 2)
        error_exit("Arithmetic expression required");

    input_str = argv[1];

    double res = parse();
    puts("===========================");
    printf("Result : %.10g\n", res);

    return 0;
}

////////////////////////////  stack  ///////////////////////////////

void push(token *tok)
{
    struct stack *p = malloc(sizeof(struct stack));
    p->token = tok;
    if (sp == NULL) {
        sp = p;
        sp->prev = NULL;
        return;
    }
    p->prev = sp;
    sp = p;
}

token* pop()
{
    if (sp == NULL) return NULL;
    token *tok = sp->token;
    struct stack *tmp = sp->prev;
    free(sp);
    sp = tmp;
    return tok;
}

token* peek()
{
    return (sp == NULL ? NULL : sp->token);
}

