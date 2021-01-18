## Arithmetic Parser

`C` 언어를 이용한 사칙연산 recursive descent parser 입니다.

The Casterian 블로그의 [BNF와 재귀 하향 파서](https://casterian.net/archives/421)
페이지에서 자세한 설명을 볼 수 있습니다.
설명에는 `C++` 로 작성하신 것을 `C` 로 다시 작성한 것입니다.
`C++` 언어가 컴파일러에서 제공하는 서비스가 많기 때문에 요즘은 대부분 `C++` 로 학습을
시작해서 그런지 왜 `C++` 가 프로그래밍에 좋은지 잘 모르는 경우가 있는것 같습니다.
여기에 있는 예제 같은 프로그램을  `C` 로 한번 작성해보면 왜 `C++` 언어가
만들어지게 됐는지 바로 알 수 있습니다.
블로그에 있는 `C++` 코드와 한번 비교해 보세요.

두 번째 parser2.c 는 따로 abstract syntax tree 를 만들지 않고 바로 계산하면서 올라갑니다.


```sh
$ gcc parser.c -lm

$ ./a.out '100 + 200 * 300' 
==========  tokenize()  =========
number
blank
plus
blank
number
blank
asterisk
blank
number
total tokens : 5
value : 100
value : +
value : 200
value : *
value : 300
===========  parse()  ===========
RUN parse_expr()
RUN parse_term()
RUN parse_factor()
RUN parse_primary_expr()
make_literal_node : 100
RUN parse_term()
RUN parse_factor()
RUN parse_primary_expr()
make_literal_node : 200
RUN parse_factor()
RUN parse_primary_expr()
make_literal_node : 300
make_binary_node eval_mul
make_binary_node eval_add
============  eval()  ===========
free literal_node : 100
free literal_node : 200
free literal_node : 300
free binary_node eval_mul
free binary_node eval_add
result : 60100
```

