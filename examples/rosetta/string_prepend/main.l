import extensions.

program =
[
    var s := "World".
    s := "Hello " + s.
    console writeLine:s.
    
    // Alternative way
    var s2 := String new:"World".
    s2 insert:"Hello " at:0.
    console writeLine:s2.
    console readChar.
].