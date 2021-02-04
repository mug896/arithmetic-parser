
# LL Parser


LL 파싱은 기본적으로 left recursion 을 제거하고 left factoring 을 해줘야 합니다.   
다음의 경우 right recursion 으로 작성되어 문제가 없지만 left factoring 이 되어있지 않습니다.

```
    E -> T - E
    E -> T + E
    E -> T
    T -> F * T
    T -> F / T
    T -> F % T
    T -> F
    F -> - F
    F -> + F
    F -> P ^ F
    F -> P
    P -> ( E )
    P -> a
```

따라서 다음과 같이 left factoring 을 해줘야 합니다.

```
    E  ->  T E'                    // 공통인수 T 를 하나만 남기고
    E' ->  - E | + E | e           // 나머지를 E' 로 분리한다.
    T  ->  F T'                    // 공통인수 F 를 하나만 남기고
    T' ->  * T | / T | % T | e     // 나머지를 T' 로 분리한다.
    F  ->  - F | + F | P F'
    F' ->  ^ F | e                 // e 는 epsilon
    P  ->  ( E ) | a               // a 는 NUMBER 입니다.
```

```
    FIRST(E)   = { - , + , ( , a }
    FIRST(E')  = { - , + , e }
    FIRST(T)   = { - , + , ( , a }
    FIRST(T')  = { * , / , % , e }
    FIRST(F)   = { - , + , ( , a }
    FIRST(F')  = { ^ , e }
    FIRST(P)   = { ( , a }
    
    FOLLOW(E)  = { ) , $ }
    FOLLOW(E') = { ) , $ }
    FOLLOW(T)  = { - , + , ) , $ }
    FOLLOW(T') = { - , + , ) , $ }
    FOLLOW(F)  = { * , / , % , - , + , ) , $ }
    FOLLOW(F') = { * , / , % , - , + , ) , $ }
    FOLLOW(P)  = { ^ , * , / , % , - , + , ) , $ }
```

```
    LOOAHEAD(E  -> T E')   = { - , + , ( , a }
    LOOAHEAD(E' -> - E)    = { - }
    LOOAHEAD(E' -> + E)    = { + }
    LOOAHEAD(E' -> e)      = { ) , $ }
    LOOAHEAD(T  -> F T')   = { - , + , ( , a }
    LOOAHEAD(T' -> * T)    = { * }
    LOOAHEAD(T' -> / T)    = { / }
    LOOAHEAD(T' -> % T)    = { % }
    LOOAHEAD(T' -> e)      = { - , + , ) , $ }
    LOOAHEAD(F  -> - F)    = { - }
    LOOAHEAD(F  -> + F)    = { + }
    LOOAHEAD(F  -> P F')   = { ( , a }
    LOOAHEAD(F' -> ^ F)    = { ^ }
    LOOAHEAD(F' -> e)      = { * , / , % , - , + , ) , $ }
    LOOAHEAD(P  -> ( E ))  = { ( }
    LOOAHEAD(P  -> a)      = { a }
```
