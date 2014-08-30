#ifndef SYSEX_PARSER_H
#define SYSEX_PARSER_H

#include <glibmm.h>
#include <iostream>

class Parser:public

{

  public:
    Parser ();
    virtual ~
    Parser ();

    char* parse_message (char* buffer, int length);

  protected:
};

#endif	// SYSEX_PARSER_H
