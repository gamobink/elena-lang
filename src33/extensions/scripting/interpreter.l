import system'text.
import extensions.
import system'dynamic.

// --- IOException ---

class ScriptEngineException :: Exception
{
    constructor new literal:l
        <= new literal:l.
}

// --- scriptEngine ---

class ScriptEngine
{
    int scopeId.
    
    constructor new
    [
        scopeId := system'external'elenasm NewScope.
    ]
    
    stacksafe $lastScriptError
    [
        byte<256> aBuffer.
        int       aLength := system'external'elenasm GetStatus
                                bytearray:aBuffer
                                int:255.
            
        ^ UTF8Encoding toLiteral int:0 int:aLength bytearray:aBuffer
    ]
    
    stacksafe $lastVMError
    [
        dirty_ptr err := system'external GetVMLastError.
            
        ^ err literal.  
    ]
    
    stacksafe evaluateTape dirty_ptr:tape
        => system'core_routines'__vmtape.
    
    stacksafe evaluateScript literal:aScript
    [
        dirty_ptr tape := system'external'elenasm InterpretScopeScript
                                int:scopeId
                                literal:aScript.
            
        if (tape isEmpty)
            [ ScriptEngineException new($self $lastScriptError); raise ].
            
        var retVal := $self evaluateTape dirty_ptr:tape.
        
        system'external'elenasm Release dirty_ptr:tape.
        
        if($nil == retVal)
            [ ScriptEngineException new($self $lastVMError); raise ].
            
        ^ retVal
    ]

    stacksafe loadScriptFile literal:path int:anEncoding int:anAutoDetect
    [
        dirty_ptr tape := system'external'elenasm InterpretScopeFile
                            int:scopeId
                            literal:path
                            int:anEncoding
                            int:anAutoDetect.
            
        if (tape isEmpty)
            [ ScriptEngineException new($self $lastScriptError); raise ].
            
        system'external'elenasm Release dirty_ptr:tape.            
    ]
    
    stacksafe evalScriptFile literal:path int:anEncoding int:anAutoDetect
    [
        dirty_ptr tape := system'external'elenasm InterpretScopeFile
                            int:scopeId
                            literal:path
                            int:anEncoding
                            int:anAutoDetect.
            
        if (tape isEmpty)
            [ ScriptEngineException new($self $lastScriptError); raise ].
            
        var retVal := $self evaluateTape dirty_ptr:tape.
        
        system'external'elenasm Release dirty_ptr:tape.
        
        if ($nil == retVal)
            [ ScriptEngineException new literal:($self $lastVMError); raise ].
            
        ^ retVal
    ]
        
    evaluate : aScript
        = $self evaluateScript literal:aScript.
        
    load path:aPath
    [
        $self loadScriptFile literal:aPath int:0 int:-1.
    ]
        
    eval path:aPath
        = $self evalScriptFile literal:aPath int:0 int:-1.
}

// --- globlas ---

static globals = system'collections'Dictionary new.