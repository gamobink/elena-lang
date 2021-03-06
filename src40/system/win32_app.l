// --- CommandLine ---

singleton CommandLineFactory
{
    wide load()
    {
        dirty_ptr commandLinePtr := extern KERNEL32.GetCommandLineW();

        ^ cast wide(commandLinePtr);
    }
}

public commandLine = 
    CommandLineFactory.load();

//// --- startUp ---
//
//class StartUpEvents
//{
//    T<Func1> event onStop :: theFinializeEvent.
//    
//    /*internal */stopping : arg
//    [
//        ifnot (nil == theFinializeEvent)
//            [ theFinializeEvent(arg) ]
//    ]
//}
//
//public static startUpEvents = StartUpEvents new.

public symbol startUp =
    {
        start(/*StartUpEvents events*/)
        {
            intern system'core_routines'__exception.register();
            
            try
            {
                forward program()
            }
            catch(Exception e)
            {
                console.writeLine(e.Printable);
            }
            catch(AbortException e)
            {
            };

//            events stopping:nil
        }        
    }
    .start()/*:startUpEvents*/;
