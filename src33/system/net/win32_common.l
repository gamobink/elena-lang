const int INVALID_SOCKET = -1.
const int SOCKET_ERROR   = -1.

const int AI_PASSIVE  = 1.
const int AF_UNSPEC   = 0.
const int AF_INET     = 2.
const int SOCK_STREAM = 1.
const int IPPROTO_TCP = 6.

const int TCP_NODELAY = 1. 

const int FIONBIO = 8004667Eh.

const int WSAEWOULDBLOCK = 10035.
const int WSAECONNRESET  = 10054.

const int SOMAXCONN = 5.

const int SD_BOTH = 2.

// --- types ---

type socket   = system'net'Socket.
type wsadata  = system'net'WSADATA.
type addrinfo = system'net'AddrInfo.

// --- WSAException ---

class SocketException :: Exception
{
    object theCode.
    
    /// Creates the exception
    constructor new:message error_code:aCode
        <= new literal:message;
    [
        theCode := aCode.
    ]
    
    constructor stacksafe new:message
        <= new literal:message;
    [
        theCode := IntNumber new int(system'external'WS2_32 WSAGetLastError).
    ]
    
    int error_code = theCode int.
    
    /// Returns the message and the call stack
    literal = theMessage add literal:$10"Socket Error code:"; add literal(theCode literal); add literal:$10"Call stack:"$10; add literal(theCallStack literal).
}

// --- WSAStartUp ---

sealed struct WSADATA
{
    short     wVersion.
    short     wHighVersion.
    byte<257> szDescription.
    byte<129> szSystemStatus.
    short     iMaxSockets.
    short     iMaxUdpDg.
    int       lpVendorInfo.
    
    stacksafe constructor init byte:aVersion byte:aHighVersion
    [
        short wVersionRequested := aVersion * 10h + aHighVersion.

        int aRetVal := system'external'WS2_32 WSAStartup 
            short:wVersionRequested
            wsadata:$self.
            
        ifnot (0 == aRetVal)
            [ SocketException new:"Cannot start socket service" error_code(IntNumber new int:aRetVal); raise ].
            
        startUpEvents onStop(:e)[ $self close ].
    ]
    
    wsadata = $self.
    
    close
    [
        system'external'WS2_32 WSACleanup.
    ]
}

preloaded static WSAStartUp22 = WSADATA init byte:2 byte:2.
