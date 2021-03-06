import system'routines.
import extensions.
import cellular.

const literal sample = 
" tH......
.        ......
 ...Ht...      .
              ....
              .  .....
              ....
 ......tH      .
.        ......
 ...Ht...".

const literal conductorLabel = ".".
const literal headLabel = "H".
const literal tailLabel = "t".
const literal emptyLabel = " ".

const int empty = 0.
const int conductor = 1.
const int electronHead = 2.
const int electronTail = 3.

wireWorldRuleSet = RuleSet::
{
    proceed(Space s, int x, int y, ref<int> retVal)
    [
        int cell := s getAt(x, y).
        
        cell =>
            conductor 
            [
                int number := s getLiveCell(x, y, electronHead).
                if ((number == 1)||(number == 2))
                [
                    retVal value := electronHead
                ];
                [
                    retVal value := conductor
                ]                
            ];
            electronHead 
            [
                retVal value := electronTail
            ];
            electronTail
            [
                retVal value := conductor
            ];
            ![
                retVal value := cell
            ].
    ]
}.

sealed class Model
{
    T<Space>   theSpace.
    
    constructor load(LiteralValue stateString,int maxX, int maxY)
    [
        var strings := stateString split(newLine); selectBy(:s)(s toArray); toArray.
        
        theSpace := IntMatrixSpace new(maxX, maxY, RuleSet::
        {
            proceed(Space s, int x, int y, ref<int> retVal)
            [
                if (x < strings length)
                [
                    var l := strings[x].
                    if (y < l length)
                    [
                        (l[y]) =>
                            conductorLabel [ retVal value := conductor ];
                            headLabel      [ retVal value := electronHead ];
                            tailLabel      [ retVal value := electronTail ];
                            emptyLabel     [ retVal value := empty ].
                    ];
                    [
                        retVal value := empty
                    ]                    
                ];
                [
                    retVal value := empty
                ]
                
            ]
        }).
    ]
    
    run
    [
        theSpace update(wireWorldRuleSet).
    ]    
    
    print
    [
        int columns := theSpace columns.
        int rows := theSpace rows.
        
        int i := 0.
        int j := 0.
        while (i < rows)
        [
            j := 0.
            
            while (j < columns)
            [
                var label := emptyLabel.
                int cell := theSpace getAt(i, j).
                
                cell =>
                    conductor    [ label := conductorLabel ];
                    electronHead [ label := headLabel ];
                    electronTail [ label := tailLabel ].
                
                console write(label).
                
                j := j + 1.
            ].
            
            i := i + 1.
            console writeLine.
        ].
    ]
}

public program
[
    T<Model> model := Model load(sample,10,30).
    0 to:10 do(:i)
    [
        console printLineFormatted("Iteration {0}",i).
        model print; run.
    ]    
]