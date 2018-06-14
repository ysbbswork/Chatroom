


#ifndef SOCKETLIBTYPES_H
#define SOCKETLIBTYPES_H


#ifdef WIN32                // windows 95 and above
    #include "winsock2.h"       // almost everything is contained in one file.
    #include "Ws2tcpip.h"

    #ifndef socklen_t
        typedef int socklen_t;
    #endif

#else                       // UNIX/Linux
    #include <sys/types.h>      // header containing all basic data types and
                                // typedefs
    #include <sys/socket.h>     // header containing socket data types and
                                // functions
    #include <netinet/in.h>     // IPv4 and IPv6 stuff
    #include <unistd.h>         // for gethostname()
    #include <netdb.h>          // for DNS - gethostbyname()
    #include <arpa/inet.h>      // contains all inet_* functions
    #include <errno.h>          // contains the error functions
    #include <fcntl.h>          // file control
#endif



namespace SocketLib
{
    // ========================================================================
    //  Globals and Typedefs
    // ========================================================================
    #ifdef WIN32                // windows 95 and above

        typedef SOCKET sock;        


    #else                       // UNIX/Linux
        typedef int sock;           // see the description above.

    #endif


    typedef unsigned short int port;        // define the port type.
    typedef unsigned long int ipaddress;    // an IP address for IPv4


}   


#endif
