#include "parser.hh"
#include <iostream>
#include <glibmm/ustring.h>
#include <sstream>
#include <string>

using std::cout;

Parser::Parser ()
{
    std::cout << "**\treFactor constructor" << std::endl;
}

Parser::~Parser ()
{
    // delete pTop;
}

bool Parserr::parse_message (char* buffer, int length)
{
    Glib::ustring text;
    std::string s;
    std::stringstream strStream (std::stringstream::in | std::stringstream::out);

    int i;
    short asc;
    bool started;

    started = true;

    if (length > 5)
	{
	    if (buffer[0] != -16)
		{
		    strStream << "Expected a SYSEX message" << '\n';
		}
	    else if ((buffer[1] != 28) || (buffer[2] != 'p') || (buffer[3] != 1))
		{
		    strStream << "Unknown Device" << '\n';
		}
		
	    switch (buffer[4])
		{
		case 'I':
		    strStream << "Presets" << '\n';
		    break;
		case 'O':
		    strStream << "Current" << '\n';
		    break;
		case 'M':
		    strStream << "System" << '\n';
		    break;
		case 'Q':
		    strStream << "All" << '\n';
		    break;
		}
	}

    for (i = 5; i < result; i++) 
	{

	    if (isprint(buffer[i]))
		{
		    if (!started) 
			{
			    strStream << '\n' << "[" << i << "]" << '\t' << "'";
			    started = true;
			}
		    
		    strStream << buffer[i];			    
		} else
		{
		    if (started)
			{
			    strStream << "'";
			    started = false;
			    strStream << '\n' << "[" << i << "]" << '\t';
			}
		    
		    asc = buffer[i];
		    strStream << "<" << asc << ">";
		}
	}

    if (started)
	{
	    strStream << "'";
	}

    strStream << '\n';
    s = strStream.str();

    std::cout << '\n' << "strStream = " << '\n' << s << '\n';
    pOutputBuffer->set_text (s);

    return 0;
}

