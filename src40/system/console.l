// --- IConsoleReader ---

public interface IConsoleReader
{
    abstract string readLine() {}
    
    abstract wide readWideLine() {}
    
//    abstract read(ref<int> retVal) []
    
    abstract read(ref char retVal) {}
    
//    abstract bool isKeyAvailable []
}

// --- Console ---

/// A console
class Console
{
    object         theWriter;
    IConsoleReader theReader;

    /// Creates a console
    constructor(reader, writer)
    {
        theWriter := writer;
        theReader := reader
    }

    /// Writes the specified literal value, followed by the current line terminator, to the standard output stream.
    writeLine(o)
    {
        self.writeLine(o.Printable)
    }

    /// Writes the specified literal value to the standard output stream.
    write(o)
    {
        self.write(o.Printable)
    }

    /// Writes the specified literal value to the standard output stream.
    write(string s)
    {
        theWriter.write(s);
        
        theWriter.refresh();
    }

//    /// Writes the specified literal value to the standard output stream.
//    write(wide aLiteral)
//    [
//        theWriter write(aLiteral).
//        
//        theWriter refresh.
//    ]

    /// the current line terminator to the standard output stream.
    writeLine(string s)
    {
        theWriter.writeLine(s);
        
        theWriter.refresh();
    }
    
    /// the current line terminator to the standard output stream.
    writeLine()
    {
        theWriter.writeLine();
        
        theWriter.refresh();
    }

//    /// Clears the console
//    clear 
//        => theWriter.
//
//    setCursorPosition(int x, int y)
//        => theWriter.

    /// Reads the next line of characters from the standard input stream.
    string readLine()
        = theReader.readLine();

//    /// Reads the next line of characters from the standard input stream.
//    wide readWideLine
//        = theReader readWide.

    /// Reads the next character from the input stream
    char readChar()
    {
        char ch := $0;
        theReader.read(ref ch);
        
        ^  ch
    }

//    bool isKeyAvailable
//        => theReader.
//
//    /// Reads the next key code from the input stream
//    readKey
//    [
//        int key := 0.
//        theReader read(&key).
//        
//        ^ key
//    ]
}

/// A standard console
public static console = new Console(io'stdConsoleReader, io'stdConsoleWriter);
