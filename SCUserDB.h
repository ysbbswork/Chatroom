


#ifndef SCUSERDB_H
#define SCUSERDB_H


#include "SocketLib/SocketLib.h"
#include <string>
#include <list>
#include<map>

using std::string;
using SocketLib::Connection;
using SocketLib::Telnet;



class User
{
public:


    User() { connection = 0; };


    User( Connection<Telnet>* c, string& n )
        : connection( c ),
          name( n )
    {
        // no code
    }

    string name;
    Connection<Telnet>* connection;
};



class UserDatabase
{
public:
	//这样写可以使DB类更像常规的STL容器
    typedef std::list<User> users;
    typedef std::list<User>::iterator iterator;

    static iterator begin() { return m_users.begin(); }


    static iterator end() { return m_users.end(); }

    static iterator find( Connection<Telnet>* p_connection );

    static bool AddUser( Connection<Telnet>* p_connection, string p_name );

    static void DeleteUser( Connection<Telnet>* p_connection );

    static bool HasUser( string& p_name );

    static bool IsValidName( const string& p_name );



protected:

    static users m_users;
};


#endif
