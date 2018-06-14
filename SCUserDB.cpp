

#include "SCUserDB.h"
#include <cctype>

using std::string;
using std::map;


UserDatabase::users UserDatabase::m_users;


UserDatabase::iterator UserDatabase::find( Connection<Telnet>* p_connection )
{

    iterator itr = m_users.begin();
    
    while( itr != m_users.end() )
    {

        if( itr->connection == p_connection ) 
            return itr;

        ++itr;
    }


    return itr;
}


bool UserDatabase::AddUser( Connection<Telnet>* p_connection, string p_name )
{
    if( !HasUser( p_name ) && IsValidName( p_name ) )
    {
        m_users.push_back( User( p_connection, p_name ) );
        return true;
    }
    return false;
}


void UserDatabase::DeleteUser( Connection<Telnet>* p_connection )
{
    iterator itr = find( p_connection );
    if( itr != m_users.end() )
        m_users.erase( itr );
}



bool UserDatabase::HasUser( string& p_name )
{
    iterator itr = m_users.begin();
    
    while( itr != m_users.end() )
    {
        if( itr->name == p_name ) return true;
        ++itr;
    }

    return false;
}



bool UserDatabase::IsValidName( const string& p_name )
{
    static string inv = " \"'~!@#$%^&*+/\\[]{}<>()=.,?;:";

    // must not contain any invalid characters,排除无效字符的好方法！
    if(  p_name.find_first_of( inv ) != string::npos )
    {
        return false;
    }

    // must be less than 16 chars
    if( p_name.size() > 16 || p_name.size() < 3 )
    {
        return false;
    }

    // must start with an alphabetical character
    if( !isalpha( p_name[0] ) )
    {
        return false;
    }

    return true;
}
