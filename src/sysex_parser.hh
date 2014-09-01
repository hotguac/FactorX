#ifndef SYSEX_PARSER_H
#define SYSEX_PARSER_H

#include <string>

typedef signed short int HeaderType;

class Parser

{
  public:
    Parser ();
    virtual ~
    Parser ();

    std::string parse_message (char* buffer, int length);

  protected:
    static const HeaderType header_type_current = 0;
    static const HeaderType header_type_presets = 1;
    static const HeaderType header_type_system  = 2;
    static const HeaderType header_type_all     = 3;

    static const HeaderType header_type_unknown    = -1;
    static const HeaderType header_type_bad_message = -2;
    static const HeaderType header_type_bad_device = -3;
    static const HeaderType header_type_not_sysex = -4;

    std::string parse_current ();
    HeaderType parse_header ();
    std::string segment_get_next ();
    std::string segment_get_next_field ();

    int segment_length;
    std::string next_segment;

    char* buffer;
    int length;
    int position;

    std::string current_segment;
};

#endif	// SYSEX_PARSER_H
