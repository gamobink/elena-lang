import system'collections.
import system'routines.
import extensions.
import extensions'routines.

type charmatrix = matrix<CharValue>.

const image = (
        "                                                          ",
        " #################                   #############        ",
        " ##################               ################        ",
        " ###################            ##################        ",
        " ########     #######          ###################        ",
        "   ######     #######         #######       ######        ",
        "   ######     #######        #######                      ",
        "   #################         #######                      ",
        "   ################          #######                      ",
        "   #################         #######                      ",
        "   ######     #######        #######                      ",
        "   ######     #######        #######                      ",
        "   ######     #######         #######       ######        ",
        " ########     #######          ###################        ",
        " ########     ####### ######    ################## ###### ",
        " ########     ####### ######      ################ ###### ",
        " ########     ####### ######         ############# ###### ",
        "                                                          ").

nbrs = ((0, -1), (1, -1), (1, 0), (1, 1), (0, 1),
        (-1, 1), (-1, 0), (-1, -1), (0, -1)).

nbrGroups = (((0, 2, 4), (2, 4, 6)), ((0, 2, 6),
        (0, 4, 6))).

charmatrix extension zhangsuenOp
{
    $proceed : r : c : toWhite : firstStep
    [
        if (self[r][c] != $35)
            [ ^ false ].
        
        int nn := self numNeighbors(r,c).
        
        if ((nn < 2) || (nn > 6))
            [ ^ false ].
            
        if(self numTransitions(r,c) != 1)
            [ ^ false ].

        ifnot (self atLeastOneIsWhite(r,c,firstStep iif(0,1)))
            [ ^ false ].

        toWhite append:{ x = c. y = r. }.
        
        ^ true.      
    ]
    
    numNeighbors :r : c
    [
        int count := 0.
        
        0 till(nbrs length - 1) do(:i)
        [
            if (self[r + nbrs[i][1]][c + nbrs[i][0]] == $35)
                [ count := count + 1. ].
        ].
                
        ^ count.
    ]

    numTransitions : r : c
    [
        int count := 0.
        
        0 till(nbrs length - 1) do(:i)
        [
            if (self[r + nbrs[i][1]][c + nbrs[i][0]] == $32)
            [
                if (self[r + nbrs[i + 1][1]][c + nbrs[i + 1][0]] == $35)
                [
                    count := count + 1.
                ].
            ].
        ].
                
        ^ count.
    ]  
    
    atLeastOneIsWhite : r : c : step
    [
        int count := 0.
        var group := nbrGroups[step].
        0 till:2 do(:i)
        [            
            0 till(group[i] length) seek(:j)
            [
                var nbr := nbrs[group[i][j]].
                
                if (self[r + nbr[1]][c + nbr[0]] == $32)
                    [ count := count + 1. ^ true ].
                    
                ^ false.                    
            ].
        ].
        
        ^ count > 1.
    ]    
    
    thinImage
    [
        bool firstStep := false.
        bool hasChanged := true.
        var toWhite := List new.

        while (hasChanged || firstStep)
        [
            hasChanged := false.
            firstStep := firstStep not.

            1 till(self rows - 1) do(:r) 
            [
                1 till(self columns - 1) do(:c)
                [
                    if(self~zhangsuenOp $proceed(r,c,toWhite,firstStep))
                        [ hasChanged := true ].
                ].
            ].
            
            toWhite forEach(:p)[ self[p y][p x] := $32. ].                
            toWhite clear.
        ].
    ]
    
    print
    [
        var it := self enumerator.
        
        it forEach(:ch) [ console print(ch," ") ].
        while (it next)
        [
            console writeLine.
            
            it forEach(:ch) [ console print(ch," ") ].
        ].
    ]
}

program =
[
    charmatrix grid := MatrixSpace::
    {
        rows = image length.
        
        columns = image[0] length.
        
        getAt int:i int:j
            = image[i][j].
    }.

    grid thinImage.
            
    grid print.
    
    console readChar.
].