
# LL Parser


LL 파싱은  다음과 같이 작성하면 안됩니다.

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

LL 파싱은 left recursion 을 제거하고 left factoring 을 해줘야 합니다.

```
    E  ->  TE'
    E' ->  - E | + E | e
    T  ->  FT'
    T' ->  * T | / T | % T | e
    F  ->  - F | + F | PF'
    F' ->  ^ F | e
    P  ->  ( E ) | a
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
    LOOAHEAD(E  -> TE')   = { - , + , ( , a }
    LOOAHEAD(E' -> - E)   = { - }
    LOOAHEAD(E' -> + E)   = { + }
    LOOAHEAD(E' -> e)     = { ) , $ }
    LOOAHEAD(T  -> FT')   = { - , + , ( , a }
    LOOAHEAD(T' -> * T)   = { * }
    LOOAHEAD(T' -> / T)   = { / }
    LOOAHEAD(T' -> % T)   = { % }
    LOOAHEAD(T' -> e)     = { - , + , ) , $ }
    LOOAHEAD(F  -> -F)    = { - }
    LOOAHEAD(F  -> +F)    = { + }
    LOOAHEAD(F  -> PF')   = { ( , a }
    LOOAHEAD(F' -> ^ F)   = { ^ }
    LOOAHEAD(F' -> e)     = { * , / , % , - , + , ) , $ }
    LOOAHEAD(P  -> (E))   = { ( }
    LOOAHEAD(P  -> a)     = { a }
```
