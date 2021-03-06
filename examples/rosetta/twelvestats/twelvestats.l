import system'routines.
import extensions.

extension op
{
    printSolution : bits
        = self zip:bits by
            (:s:b)( s iif("T","F") + (s xor:b) iif("* ","  ") ); summarize(String new).

    toBit
        = self iif(1,0).
}

puzzle =
(
    (:bits)( bits length == 12 ),
    
    (:bits)( bits last:6; selectBy(:x)( x toBit ); summarize == 3 ),
    
    (:bits)( bits zip(RangeEnumerator new from:1 to:12) 
                        by(:x:i)( (i int; isEven)and:x; toBit ); summarize == 2 ),
    
    (:bits)( bits[4] iif(bits[5] && bits[6],true) ),
    
    (:bits)( (bits[1] || bits[2] || bits[3]) not ),
    
    (:bits)( bits zip(RangeEnumerator new from:1 to:12) 
                        by(:x:i)( (i int; isOdd)and:x; toBit ); summarize == 4 ),
    
    (:bits)( bits[1] xor(bits[2]) ),
    
    (:bits)( bits[6] iif(bits[5] && bits[4],true) ),
    
    (:bits)( bits top:6; selectBy(:x)( x toBit ); summarize == 3 ),
    
    (:bits)( bits[10] && bits[11] ),
    
    (:bits)( (bits[6] toBit + bits[7] toBit + bits[8] toBit)==1 ),
    
    (:bits)( bits top:11; selectBy(:x)( x toBit ); summarize == 4 )
).

program =
[
    console writeLine:"".
    
    0 till(2 power int:12) do(:n)
    [
        var bits := BitArray32 new:n; top:12; toArray.
        var results := puzzle selectBy(:r)( r eval:bits ); toArray.
        
        var counts := bits zip:results by(:b:r)( b xor:r; toBit ); summarize.

        counts =>
            0  [ console printLine("Total hit :",results printSolution:bits) ];
            1  [ console printLine("Near miss :",results printSolution:bits) ];
            12 [ console printLine("Total miss:",results printSolution:bits) ].
    ].
    
    console readChar.
].