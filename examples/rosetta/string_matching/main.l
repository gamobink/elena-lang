import extensions.

program =
[
    var s := "abcd".
    
    console printLine(s," starts with ab: ",s startingWith:"ab").
    console printLine(s," starts with cd: ",s startingWith:"cd").
    
    console printLine(s," ends with ab: ",s endingWith:"ab").
    console printLine(s," ends with cd: ",s endingWith:"cd").
    
    console printLine(s," contains ab: ",s containing:"ab").
    console printLine(s," contains bc: ",s containing:"bc").
    console printLine(s," contains cd: ",s containing:"cd").
    console printLine(s," contains az: ",s containing:"az").
    
    console printLine(s," index of az: ",s indexOf:"az" at:0).
    console printLine(s," index of cd: ",s indexOf:"cd" at:0).
    console printLine(s," index of bc: ",s indexOf:"bc" at:0).
    console printLine(s," index of ab: ",s indexOf:"ab" at:0).
    
    console readChar.
].