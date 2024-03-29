# LR Paser


LR 파싱은 stack 을 이용하는 automaton 을 만들어서 기계적으로 처리하는 방법입니다.
자판기를 예로 들면 유한한 상태를 가지고 deterministic 하게 동작하죠.
LR 파싱의 경우에는 stack 에 토큰이 하나 들어오면 상태가 변하게 되는데 이것을 모두 트래킹 해서 action 과 goto 테이블을 만듭니다.
LR_parser.c 파일에서 실제 파싱을 처리하는 부분을 보면
미리 만들어놓은 테이블을 참조해서 기계적으로 shift 와 reduce 하는 것뿐이 없습니다.

따라서 LR 파싱에서 주된 작업은 파싱 테이블 ( 일종의 DFA ) 을 만드는 것입니다.
먼저 사용할 rule 을 작성한 다음 작성한 rule 에 따라서 파싱 테이블을 만들게 되는데
이것도 작성해보면 기계적인 작업입니다.
따라서 사람이 rule 만 작성해놓으면 자동으로 파싱 테이블을 만들어 주는 툴이 있는데
이것이 yacc ( bison ) 파서 생성기입니다.

LR 파싱은 LL 파싱과 다르게 rule 을 작성할때 기본적으로 left recursion 을 사용해야 합니다.
그래야 reduce 가 되면서 진행이 됩니다.
만약에 right recursion 을 사용하게 되면 입력 토큰이 모두 consume 될 때까지 reduce 가 
일어나지 않습니다.
하지만 이것이 유용하게 사용될 때가 있는데 `^` 연산자 에서처럼 right associativity 
를 구현할 때입니다.
예를 들어 `4 ^ 3 ^ 2` 식이 있을 경우 덧셈이나 곱셈에서처럼 `4 ^ 3` 에서 reduce 가 일어나면 안되겠죠. 
이때 `^` 연산자의 rule 에 right recursion 을 사용하게 되면 `^ 2` 까지 모두 읽어들인 후에
오른쪽부터 reduce 가 일어나게 됩니다.



파싱 테이블을 어떻게 만들고 사용하는지는 Joongheon Kim 님의 강좌에서 볼 수 있습니다.

- Joongheon Kim 님의 컴파일러 강좌  
https://www.youtube.com/watch?v=MAG4ten4nAM&list=PLalDxVXf3NHertbSsvTLOLZz0T3FyCQnI

- 강승식 교수님의 컴파일러 강좌   
https://www.youtube.com/channel/UC9BQ1zJdpPBit9V3IFx8GDw/videos

- LL and LR Parsing Demystified  
https://blog.reverberate.org/2013/07/ll-and-lr-parsing-demystified.html

- Rule 을 입력하면 자동으로 파싱 테이블을 만들어주는 사이트  
[SLR](http://jsmachines.sourceforge.net/machines/slr.html) ,
[LR1](http://jsmachines.sourceforge.net/machines/lr1.html) ,
[LALR1](http://jsmachines.sourceforge.net/machines/lalr1.html) ,
[소스코드](http://jsmachines.sourceforge.net/machines/)

- 참고 동영상 :
[lookahead](https://www.youtube.com/watch?v=9BpEbPpYUHs&list=PLywg83lhcrgDvPgTr-xatGWSe6YYnDevz&index=95&t=388s) ,
[LR1](https://www.youtube.com/watch?v=GyvpUVGrR8A&list=PLywg83lhcrgDvPgTr-xatGWSe6YYnDevz&index=96) ,
[LALR1](https://www.youtube.com/watch?v=DYe1RyDBRMw&list=PLywg83lhcrgDvPgTr-xatGWSe6YYnDevz&index=97&t=8s) ,
[Conflicts](https://www.youtube.com/watch?v=Nxj0g1mk5Ak&list=PLywg83lhcrgDvPgTr-xatGWSe6YYnDevz&index=98&t=181s)


LR 파싱은 `LR0`, `SLR`, `LR1`, `LALR1`  여러 종류가 있는데 기본적으로
사용하는 알고리즘은 모두 같습니다.
단지 파싱 테이블을 만드는 방법이 틀립니다.
계산기 파서에서 사용된 방법은 SLR 인데 LR0 과의 차이점은 파싱 테이블에서 reduce 항목을
채울때 SLR 은 FOLLOW 를 계산해서 구분하지만 LR0 같은 경우는 구분없이 해당 state 의 행을
모두 다 채웁니다.
LR0, SLR 에서 사용되는 아이템을 LR0 아이템이라고 하는데 LR1, LALR1 에서는
lookahead 가 붙는 LR1 아이템을 사용합니다.
예를 들어 `T -> F.` LR0 아이템이 있을 경우 LR0 에서는 stack 의 top 에 `F` 가 있으면 
무조건 `T` 로 reduce 가 되지만 `T -> F. , a` 와같이 lookahead 가 붙는 LR1 아이템을
사용하는 LR1 에서는 다음에 오는 토큰이 `a` 일 때만 reduce 가 됩니다.

( 여기서 lookahead 는 실제 FOLLOW 와 같습니다. 따라서 위에서 파싱 테이블을 만들어
  주는 사이트에서 결과를 출력해 보면 LR1, LALR1 은 따로 FOLLOW 값이 출력되지 않습니다. )

이것은 다음에 오는 토큰까지 참조해서 상태를 더 많이 구분할 수 있다는 뜻이므로
따라서 어떤 방법이 제일 강력한가를 따질 때는 `LR0 < SLR < LALR1 < LR1` 순서가 됩니다.
LALR1 은 LR1 방식이 파싱 테이블이 너무 커지니까 
LR0 아이템은 같은데 lookahead 만 틀릴 경우 `T -> F. , a` 와 `T -> F. , b` 를 
 `T -> F. a/b` 하나로 merge 해서 상태를 줄이는 방법입니다.

```
    rule 0 :	S -> E
    rule 1 :	E -> E - T       // 기본적으로 left recursion 을 사용해야 한다.
    rule 2 :	E -> E + T          (E - T 에서 E 가 왼쪽에 위치)
    rule 3 :	E -> T
    rule 4 :	T -> T * F
    rule 5 :	T -> T / F
    rule 6 :	T -> T % F
    rule 7 :	T -> F
    rule 8 :	F -> - F
    rule 9 :	F -> + F
    rule 10 :   F -> P ^ F       // right associativity 를 위한 right recursion
    rule 11 :   F -> P              (P ^ F 에서 F 가 오른쪽에 위치)
    rule 12 :	P -> ( E )
    rule 13 :	P -> a
```

```
    FIRST(E)   = { - , + , ( , a }      // FOLLOW 를 구하기 위해선 먼저 FIRST 를 구해야 한다.
    FIRST(T)   = { - , + , ( , a }
    FIRST(F)   = { - , + , ( , a }      // a 는 NUMBER 입니다.
    FIRST(P)   = { ( , a }
    
    FOLLOW(E)  = { - , + , ) , $ }
    FOLLOW(T)  = { * , / , % , - , + , ) , $ }
    FOLLOW(F)  = { * , / , % , - , + , ) , $ }
    FOLLOW(P)  = { ^ , * , / , % , - , + , ) , $ }
```

```
	CLOSURE([S->.E]) = [S->.E] [E->.E-T] [E->.E+T] [E->.T]
	                   [T->.T*F] [T->.T/F] [T->.T%F] [T->.F]
	                   [F->.-F] [F->.+F] [F->.P] [F->.P^F]
	                   [P->.(E)] [P->.a] = <X0>
>>> GOTO for X0 
        GOTO(X0, E)  = CLOSURE([S->E.] [E->E.-T] [E->E.+T])
                     = [S->E.] [E->E.-T] [E->E.+T] = <X1>
        GOTO(X0, T)  = CLOSURE([E->T.] [T->T.*F] [T->T./F] [T->T.%F])
                     = [E->T.] [T->.*F] [T->T./F] [T->T.%F] = <X2>
        GOTO(X0, F)  = CLOSURE([T->F.]) = [T->F.] = <X3>
        GOTO(X0,'-') = CLOSURE([F->-.F]) 
                     = [F->-.F] [F->.-F] [F->.+F] [F->.P] [F->.P^F]
                       [P->.(E)] [P->.a] = <X4>
        GOTO(X0,'+') = CLOSURE([F->+.F])
                     = [F->+.F] [F->.-F] [F->.+F] [F->.P] [F->.P^F]
                       [P->.(E)] [P->.a] = <X5>
        GOTO(X0, P)  = CLOSURE([F->P.^F] [F->P.])
                     = [F->P.^F] [F->P.] = <X6>
        GOTO(X0,'(') = CLOSURE([P->(.E)])
                     = [P->(.E)] [E->.E-T] [E->.E+T] [E->.T]
                       [T->.T*F] [T->.T/F] [T->.T%F] [T->.F]
                       [F->.-F] [F->.+F] [F->.P] [F->.P^F]
                       [P->.(E)] [P->.a] = <X7>
        GOTO(X0, a)  = CLOSURE([P->a.]) = [P->a.] = <X8>
        
>>> GOTO for X1
        GOTO(X1, -)  = CLOSURE([E->E-.T])
                     = [E->E-.T] [T->.T*F] [T->.T/F] [T->.T%F] [T->.F]
                       [F->.-F] [F->.+F] [F->.P] [F->.P^F]
                       [P->.(E)] [P->.a] = <X9>
        GOTO(X1, +)  = CLOSURE([E->E+.T])
                     = [E->E+.T] [T->.T*F] [T->.T/F] [T->.T%F] [T->.F]
                       [F->.-F] [F->.+F] [F->.P] [F->.P^F]
                       [P->.(E)] [P->.a] = <X10>
	                   
>>> GOTO for X2
        GOTO(X2, *)  = CLOSURE([T->T*.F])
                     = [T->T*.F] [F->.-F] [F->.+F] [F->.P] [F->.P^F]
                       [P->.(E)] [P->.a] = <X11>
        GOTO(X2, /)  = CLOSURE([T->T/.F])
                     = [T->T/.F] [F->.-F] [F->.+F] [F->.P] [F->.P^F]
                       [P->.(E)] [P->.a] = <X12>
        GOTO(X2, %)  = CLOSURE([T->T%.F])
                     = [T->T%.F] [F->.-F] [F->.+F] [F->.P] [F->.P^F]
                       [P->.(E)] [P->.a] = <X13>
	                   
>>> GOTO for X4
        GOTO(X4, F)  = CLOSURE([F->-F.]) = [F->-F.] = <X14>
        GOTO(X4,'-') = CLOSURE([F->-.F]) = <X4>
        GOTO(X4,'+') = CLOSURE([F->+.F]) = <X5>
        GOTO(X4, P)  = CLOSURE([F->P.] [F->P.^F]) = <X6>
        GOTO(X4,'(') = CLOSURE([P->(.E)]) = <X7>
        GOTO(X4, a)  = CLOSURE([P->a.]) = <X8>

>>> GOTO for X5        
        GOTO(X5, F)  = CLOSURE([F->+F.]) = [F->+F.] = <X15>
        GOTO(X5,'-') = CLOSURE([F->-.F]) = <X4>
        GOTO(X5,'+') = CLOSURE([F->+.F]) = <X5>
        GOTO(X5, P)  = CLOSURE([F->P.] [F->P.^F]) = <X6>
        GOTO(X5,'(') = CLOSURE([P->(.E)]) = <X7>
        GOTO(X5, a)  = CLOSURE([P->a.]) = <X8>

>>> GOTO for X6
        GOTO(X6,'^') = CLOSURE([F->P^.F])
                     = [F->P^.F] [F->.-F] [F->.+F] [F->.P] [F->.P^F]
                       [P->.(E)] [P->.a] = <X16>
	                   
>>> GOTO for X7
        GOTO(X7, E)  = CLOSURE([P->(E.)] [E->E.-T] [E->E.+T])
                     = [P->(E.)] [E->E.-T] [E->E.+T] = <X17>
        GOTO(X7, T)  = CLOSURE([E->T.] [T->T.*F] [T->T./F] [T->T.%F]) = <X2>
        GOTO(X7, F)  = CLOSURE([T->F.]) = <X3>
        GOTO(X7,'-') = CLOSURE([F->-.F]) = <X4>
        GOTO(X7,'+') = CLOSURE([F->+.F]) = <X5>
        GOTO(X7, P)  = CLOSURE([F->P.] [F->P.^F]) = <X6>
        GOTO(X7,'(') = CLOSURE([P->(.E)]) = <X7>
        GOTO(X7, a)  = CLOSURE([P->a.]) = <X8>
        
>>> GOTO for X9
        GOTO(X9, T)  = CLOSURE([E->E-T.] [T->T.*F] [T->T./F] [T->T.%F])
                     = [E->E-T.] [T->T.*F] [T->T./F] [T->T.%F] = <X18>
        GOTO(X9, F)  = CLOSURE([T->F.]) = <X3>
        GOTO(X9,'-') = CLOSURE([F->-.F]) = <X4>
        GOTO(X9,'+') = CLOSURE([F->+.F]) = <X5>
        GOTO(X9, P)  = CLOSURE([F->P.] [F->P.^F]) = <X6>
        GOTO(X9,'(') = CLOSURE([P->(.E)]) = <X7>
        GOTO(X9, a)  = CLOSURE([P->a.]) = <X8>

>>> GOTO for X10
        GOTO(X10, T)  = CLOSURE([E->E+T.] [T->T.*F] [T->T./F] [T->T.%F])
                      = [E->E+T.] [T->T.*F] [T->T./F] [T->T.%F] = <X19>
        GOTO(X10, F)  = CLOSURE([T->F.]) = <X3>
        GOTO(X10,'-') = CLOSURE([F->-.F]) = <X4>
        GOTO(X10,'+') = CLOSURE([F->+.F]) = <X5>
        GOTO(X10, P)  = CLOSURE([F->P.] [F->P.^F]) = <X6>
        GOTO(X10,'(') = CLOSURE([P->(.E)]) = <X7>
        GOTO(X10, a)  = CLOSURE([P->a.]) = <X8>

>>> GOTO for X11        
        GOTO(X11, F)  = CLOSURE([T->T*F.]) = [T->T*F.] = <X20>
        GOTO(X11,'-') = CLOSURE([F->-.F]) = <X4>
        GOTO(X11,'+') = CLOSURE([F->+.F]) = <X5>
        GOTO(X11, P)  = CLOSURE([F->P.] [F->P.^F]) = <X6>
        GOTO(X11,'(') = CLOSURE([P->(.E)]) = <X7>
        GOTO(X11, a)  = CLOSURE([P->a.]) = <X8>

>>> GOTO for X12
        GOTO(X12, F)  = CLOSURE([T->T/F.]) = [T->T/F.] = <X21>
        GOTO(X12,'-') = CLOSURE([F->-.F]) = <X4>
        GOTO(X12,'+') = CLOSURE([F->+.F]) = <X5>
        GOTO(X12, P)  = CLOSURE([F->P.] [F->P.^F]) = <X6>
        GOTO(X12,'(') = CLOSURE([P->(.E)]) = <X7>
        GOTO(X12, a)  = CLOSURE([P->a.]) = <X8>

>>> GOTO for X13
        GOTO(X13, F)  = CLOSURE([T->T%F.]) = [T->T%F.] = <X22>
        GOTO(X13,'-') = CLOSURE([F->-.F]) = <X4>
        GOTO(X13,'+') = CLOSURE([F->+.F]) = <X5>
        GOTO(X13, P)  = CLOSURE([F->P.] [F->P.^F]) = <X6>
        GOTO(X13,'(') = CLOSURE([P->(.E)]) = <X7>
        GOTO(X13, a)  = CLOSURE([P->a.]) = <X8>

>>> GOTO for X16
        GOTO(X16, F)  = CLOSURE([F->P^F.]) = [F->P^F.] = <X23>
        GOTO(X16,'-') = CLOSURE([F->-.F]) = <X4>
        GOTO(X16,'+') = CLOSURE([F->+.F]) = <X5>
        GOTO(X16, P)  = CLOSURE([F->P.] [F->P.^F]) = <X6>
        GOTO(X16,'(') = CLOSURE([P->(.E)]) = <X7>
        GOTO(X16, a)  = CLOSURE([P->a.]) = <X8>

>>> GOTO for X17
        GOTO(X17,')') = CLOSURE([P->(E).]) = [P->(E).] = <X24>
        GOTO(X17,'-') = CLOSURE([E->E-.T]) = <X9>
        GOTO(X17,'+') = CLOSURE([E->E+.T]) = <X10>
        
>>> GOTO for X18
        GOTO(X18,'*') = CLOSURE([T->T*.F]) = <X11>
        GOTO(X18,'/') = CLOSURE([T->T/.F]) = <X12>
        GOTO(X18,'%') = CLOSURE([T->T%.F]) = <X13>
        
>>> GOTO for X19
        GOTO(X19,'*') = CLOSURE([T->T*.F]) = <X11>
        GOTO(X19,'/') = CLOSURE([T->T/.F]) = <X12>
        GOTO(X19,'%') = CLOSURE([T->T%.F]) = <X13>
```        
