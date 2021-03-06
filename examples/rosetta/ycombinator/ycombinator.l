import extensions.

singleton YCombinator
{
    fix : func
        = (:f)[(:x)[ x(x) ] eval(:g)[ f eval(:x)[(g(g)) eval:x] ]] eval:func.
}

program =
[
    var fib := YCombinator fix(:f)((:i)( (i <= 1) ifTrue:[^i] ifFalse:[^f(i-1) + f(i-2) ] )).
    var fact := YCombinator fix(:f)((:i)((i == 0) ifTrue:[^1] ifFalse:[^f(i-1) * i] )).
                    
    console printLine("fib(10)=",fib(10)).
    console printLine("fact(10)=",fact(10)).
].