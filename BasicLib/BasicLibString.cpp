
#include "BasicLibString.h"
#include <cctype>
#include <stdlib.h>

namespace BasicLib
{
std::string UpperCase( const std::string& p_string )
{
    std::string str = p_string;

    for( size_t i = 0; i < str.size(); i++ )
    {
        str[i] = toupper( str[i] );
    }
    return str;
}

std::string LowerCase( const std::string& p_string )
{
    std::string str = p_string;

    for( size_t i = 0; i < str.size(); i++ )
    {
        str[i] = tolower( str[i] );
    }
    return str;
}



std::string SearchAndReplace( 
    const std::string& p_target,
    const std::string& p_search,
    const std::string& p_replace )
{
    std::string str( p_target );
    std::string::size_type i = str.find( p_search );
    
    // loop while replacing all occurances
    while( i != std::string::npos )
    {
        str.replace( i, p_search.size(), p_replace );
        i = str.find( p_search, i + 1 );
    }

    return str;
}



std::string TrimWhitespace( const std::string& p_string )
{
    int wsf;
    int wsb;

    // trim the front
    wsf = p_string.find_first_not_of( WHITESPACE );
    wsb = p_string.find_last_not_of( WHITESPACE );

    if( wsf == std::string::npos )
    {
        wsf = 0;
        wsb = -1;
    }

    return p_string.substr( wsf, wsb - wsf + 1 );
}

// ------------------------------------------------------------------------
//�ڶ�������Ϊ�ҵ�p_string�еڼ����Կո�ֿ��ĵ��ʣ����硰apple cat dog��p_index=0����apple,=1 ����cat
// ------------------------------------------------------------------------
std::string ParseWord( const std::string& p_string, int p_index )
{
    int wss = p_string.find_first_not_of( WHITESPACE );

    while( p_index > 0 )
    {
        p_index--;

        
		//find �����ӵڶ�������������꿪ʼ��������first���Ǵӵڶ������������������last�ĵ��Ǵӵڶ���������ǰ����
        wss = p_string.find_first_of( WHITESPACE, wss );//������
        wss = p_string.find_first_not_of( WHITESPACE, wss );
    }
	
	int wse = p_string.find_first_of(WHITESPACE, wss);
	//˵��wssû�ҵ�����wss���Ҵ��󣬱�����������ʣ���ڶ�����������100�϶��Ҳ���
    if( wss == std::string::npos )
    {
        wss = 0;
        wse = 0;
    }


	//����ڶ�������Ϊ0�����ʾ����һ���մ�
    return p_string.substr( wss, wse - wss );
}
//ɾ��һ���Կո�ָ�ĵ��ʣ�����p_string=��apple cat dog��p_index=0���ء�cat dog��apple,=1 ���ء�apple dog��
std::string RemoveWord( const std::string& p_string, int p_index )
{
    int wss = p_string.find_first_not_of( WHITESPACE );

    while( p_index > 0 )
    {
        p_index--;

        
        wss = p_string.find_first_of( WHITESPACE, wss );
        wss = p_string.find_first_not_of( WHITESPACE, wss );
    }

    // find the end of the word
    int wse = p_string.find_first_of( WHITESPACE, wss );

    // find the beginning of the next word
    wse = p_string.find_first_not_of( WHITESPACE, wse );

    if( wss == std::string::npos )
    {
        wss = 0;
        wse = 0;
    }

    std::string str = p_string;
    str.erase( wss, wse - wss );
    return str;
}



}
