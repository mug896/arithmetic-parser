


```
    rule 0 :	S -> E
    rule 1 :	E -> E - T
    rule 2 :	E -> E + T
    rule 3 :	E -> T
    rule 4 :	T -> T * F
    rule 5 :	T -> T / F
    rule 6 :	T -> T % F
    rule 7 :	T -> F
    rule 8 :	F -> - F
    rule 9 :	F -> + F
    rule 10 :   F -> P ^ F
    rule 11 :   F -> P
    rule 12 :	P -> ( E )
    rule 13 :	P -> a
```

```
    FIRST(E)   = { - , + , ( , a }
    FIRST(T)   = { - , + , ( , a }
    FIRST(F)   = { - , + , ( , a }
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
