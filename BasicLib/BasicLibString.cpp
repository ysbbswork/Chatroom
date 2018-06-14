
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
//第二个参数为找到p_string中第几个以空格分开的单词，例如“apple cat dog”p_index=0返回apple,=1 返回cat
// ------------------------------------------------------------------------
std::string ParseWord( const std::string& p_string, int p_index )
{
    int wss = p_string.find_first_not_of( WHITESPACE );

    while( p_index > 0 )
    {
        p_index--;

        
		//find 函数从第二个参数这个坐标开始搜索，带first的是从第二个参数向后搜索，带last的的是从第二个坐标向前搜索
        wss = p_string.find_first_of( WHITESPACE, wss );//搜索到
        wss = p_string.find_first_not_of( WHITESPACE, wss );
    }
	
	int wse = p_string.find_first_of(WHITESPACE, wss);
	//说明wss没找到，即wss查找错误，比如就三个单词，你第二个参数输入100肯定找不到
    if( wss == std::string::npos )
    {
        wss = 0;
        wse = 0;
    }


	//如果第二个参数为0，则表示返回一个空串
    return p_string.substr( wss, wse - wss );
}
//删除一个以空格分割的单词，例如p_string=“apple cat dog”p_index=0返回“cat dog”apple,=1 返回“apple dog”
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
