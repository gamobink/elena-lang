const int DEFAULT_PORT = 6881.

// --- program ---

public program
[
    console writeLine:"ELENA Chat Server".
    
    auto server := ChatServer new(DEFAULT_PORT).

    server start.

    console readChar.
]