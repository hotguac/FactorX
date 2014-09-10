#include "sysex_parser.hh"

#include <glibmm/ustring.h>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

using std::cout;
using std::string;
using Glib::ustring;

Parser::Parser()
{
	std::cout << "**\treFactor constructor" << std::endl;
}

Parser::~Parser()
{
	// delete pTop;
}

HeaderType Parser::parse_header()
{
	string s;
	HeaderType result;

	result = header_type_bad_message;
	if (length > 5) {
		result = header_type_unknown;
		if (buffer[0] != -16) {
			result = header_type_not_sysex;
		} else if ((buffer[1] != 28) || (buffer[2] != 112)) { //'p')
		    //|| (buffer[3] != 1)) {
			result = header_type_bad_device;
			std::stringstream strStream(std::stringstream::in | std::stringstream::out);
			short asc;

			asc = buffer[1];
			strStream << "1<" << asc << ">";
			cout << strStream.str() << '\n';

			asc = buffer[2];
			strStream << "2<" << asc << ">";
			cout << strStream.str() << '\n';

		} else {
			switch (buffer[4]) {
			case 'I':
				result = header_type_presets;
				break;
			case 'O':
				result = header_type_current;
				break;
			case 'M':
				result = header_type_system;
				break;
			case 'Q':
				result = header_type_all;
				break;
			}
		}
	}

	position = 5;

	return result;
}

string Parser::parse_current()
{
	string s;
	string param_value;

	std::stringstream strStream(std::stringstream::in | std::stringstream::
				    out);

	int i;
	short asc;

	bool started;

	s = segment_get_next();
	std::cout << "next segment = " << s << '\n';
	strStream << "Seg1: '" << s << "'" << '\n';

	s = segment_get_next();
	std::cout << "next segment = " << s << '\n';

	param_value = segment_get_next_field();
	strStream << "Seg2: [Unknown1:'" << param_value << "] ";

	param_value = segment_get_next_field();
	strStream << "[DMOD:'" << param_value << "] ";

	param_value = segment_get_next_field();
	strStream << "[SMOD:'" << param_value << "] ";

	param_value = segment_get_next_field();
	strStream << "[ModRate:'" << param_value << "] ";

	param_value = segment_get_next_field();
	strStream << "[ModSource:'" << param_value << "] ";

	param_value = segment_get_next_field();
	strStream << "[xnob:'" << param_value << "] ";

	param_value = segment_get_next_field();
	strStream << "[unknown:'" << param_value << "] ";

	param_value = segment_get_next_field();
	strStream << "[SPEED:'" << param_value << "] ";

	param_value = segment_get_next_field();
	strStream << "[DEPTH:'" << param_value << "] ";

	param_value = segment_get_next_field();
	strStream << "[TYPE:'" << param_value << "] ";

	param_value = segment_get_next_field();
	strStream << "[ITENSITY:'" << param_value << "] ";

	param_value = segment_get_next_field();
	strStream << "[Unknown2:'" << param_value << "]";

	strStream << '\n';

	s = segment_get_next();
	std::cout << "next segment = " << s << '\n';
	strStream << "Seg3: '" << s << "'" << '\n';

	s = segment_get_next();
	std::cout << "next segment = " << s << '\n';
	strStream << "Seg4: '" << s << "'" << '\n';

	s = segment_get_next();
	std::cout << "next segment = " << s << '\n';

	param_value = segment_get_next_field();
	strStream << "Seg5: [DMOD:'" << param_value << "'] ";

	param_value = segment_get_next_field();
	strStream << "[SMOD:'" << param_value << "'] ";

	param_value = segment_get_next_field();
	strStream << "[ModRate:'" << param_value << "'] ";

	param_value = segment_get_next_field();
	strStream << "[ModSource:'";

	int x = atoi(param_value.c_str());

	switch (x) {
	case 0:
		strStream << "Sine";
		break;
	case 1:
		strStream << "Triangle";
		break;
	case 2:
		strStream << "Peak";
		break;
	case 3:
		strStream << "Random";
		break;
	case 4:
		strStream << "Ramp";
		break;
	case 5:
		strStream << "Square";
		break;
	case 6:
		strStream << "Sample/Hold";
		break;
	case 7:
		strStream << "Envelope";
		break;
	case 8:
		strStream << "ADSR";
		break;
	case 9:
		strStream << "Expression Pedal";
		break;
	default:
		strStream << "Unknown:" << param_value;
		break;
	}

	strStream << "'] ";

	param_value = segment_get_next_field();
	strStream << "[xnob FLT:'" << param_value << "'] ";

	param_value = segment_get_next_field();
	strStream << "[unknown:'" << param_value << "'] ";

	param_value = segment_get_next_field();
	strStream << "[SPEED:'" << param_value << "'] ";

	param_value = segment_get_next_field();
	strStream << "[DEPTH:'" << param_value << "'] ";

	param_value = segment_get_next_field();
	strStream << "[TYPE:'";

	x = atoi(param_value.c_str());

	switch (x) {
	case 0:
		strStream << "liquid";
		break;
	case 1:
		strStream << "organic";
		break;
	case 2:
		strStream << "shimmer";
		break;
	case 3:
		strStream << "classic";
		break;
	}

	strStream << "'] ";

	param_value = segment_get_next_field();
	strStream << "[INTENSITY:'" << param_value << "'] ";

	strStream << '\n';

	// Seg 6
	s = segment_get_next();
	std::cout << "next segment = " << s << '\n';
	strStream << "Seg6: '" << s << "'" << '\n';

	s = segment_get_next();
	std::cout << "next segment = " << s << '\n';
	strStream << "Seg7: '" << s << "'" << '\n';

	return strStream.str();
}

/*
Seg1: '[2] 0 3'
Seg2: ' 0 0 0 20 20 2ce0 de0 7fe0 19a0 7fe0 d40 0'
Seg3: ' 0 0 0 0 0 0 0 0 0 3f20 0 20 0 0 0 0 0 0 0 0'
Seg4: ' 0 1009 0 0'
Seg5: [unknown:'0'] [unknown:'0'] [unknown:'0.13'] [unknown:'0'] [unknown:'34.74'] [unknown:'0.98'] [SPEED:'5'] [DEPTH:'19.84'] [unknown:'3'] [INTENSITY:'10.26'] Seg6: 'C_ad00'
Seg7: 'Big Clone'

Seg1: '[2] 0 3'
Seg2: ' 0 0 0 20 20 5500 de0 7fe0 19a0 7fe0 d40 0'
Seg3: ' 0 0 0 0 0 0 0 0 0 3f20 0 20 0 0 0 0 0 0 0 0'
Seg4: ' 0 1009 0 0'
Seg5: [unknown:'0'] [unknown:'0'] [unknown:'0.13'] [unknown:'0'] [unknown:'65.81'] [unknown:'0.98'] [SPEED:'5'] [DEPTH:'19.84'] [unknown:'3'] [INTENSITY:'10.26'] Seg6: 'C_d53f'
Seg7: 'Big Clone'

1:'[2] 0 3'
2:' 0 0 0 20 20 0 20 20 7fe0 0 40a0 0'
3:' 0 0 0 0 0 0 0 0 0 3f20 0 20 0 0 0 0 0 0 0 0'
4:' 0 1009 0 0'
5:' 0 0 0.13 0 0 0 [SPEED:0.1] [DEPTH:99] 0.79 [INTENISTY:50.03] 65000 65000'
6:'C_cae'
7:'Big Clone'

[172]	<13><10><0><-9>
 */

std::string Parser::segment_get_next_field()
{
	string temp;
	std::size_t position;
	string result;

	temp = current_segment;

	position = temp.find(" ");

	result = temp.substr(0, position);

	current_segment = temp.substr(position + 1, string::npos);

	if ((position == 0) && (current_segment.length() > 0)) {
		result = segment_get_next_field();
	}

	return result;
}

std::string Parser::segment_get_next()
{
	string result = "";
	std::stringstream strStream(std::stringstream::in | std::stringstream::
				    out);
	int i;
	int j;
	short asc;

	i = position;

	while ((buffer[i] != 0) && (buffer[i] != 13) && (i < length)) {
		++i;
	}

	for (j = position; j < i; ++j) {
		if (isprint(buffer[j])) {
			strStream << buffer[j];
		} else {
			asc = buffer[j];
			strStream << "<" << asc << ">";
		}
	}

	result = strStream.str();
	current_segment = result;

	if (buffer[i] == 0) {
		i = length + 1;
	}

	if (i + 2 < length) {
		position = i + 2;
	}

	return result;
}

string Parser::parse_message(char *in_buffer, int in_length)
{
	buffer = in_buffer;
	length = in_length;

	std::stringstream strStream(std::stringstream::in | std::stringstream::
				    out);

	HeaderType message_type;

	message_type = parse_header();

	/*
	std::cout << "parse message length " << length << '\n';
	std::cout << "with buffer " << buffer << '\n';
	std::cout << "parse header returned " << message_type << '\n';
	*/

	switch (message_type) {
	case header_type_current:
		strStream << parse_current();
		break;
	case header_type_presets:
		break;
	case header_type_system:
		break;
	case header_type_all:
		break;
	case header_type_unknown:
		strStream << "Unknown error" << '\n';
		break;
	case header_type_bad_message:
		strStream << "Bad message" << '\n';
		break;
	case header_type_bad_device:
		strStream << "Bad device" << '\n';
		break;
	case header_type_not_sysex:
		strStream << "Expecting sysex message" << '\n';
		break;
	}

	position = 5;		// point the current position past the header

	return strStream.str();
}
