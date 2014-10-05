#ifndef SYSEX_PARSER_H
#define SYSEX_PARSER_H

#include <string>

typedef signed short int HeaderType;

class Parser {
public:
	Parser();
	virtual ~ Parser();

	std::string parse_message(char *buffer, int length);

	struct Patch {
		std::string patch_name;
		std::string effect;
		double intensity;
		 std::string type;
		double depth;
		double speed;
		 std::string shape;
		double xnob;

		double dmod;
		double smod;
		double mod_rate;
		 std::string mod_source;
	};

	Patch current;

protected:
	static const HeaderType header_type_current = 0;
	static const HeaderType header_type_presets = 1;
	static const HeaderType header_type_system = 2;
	static const HeaderType header_type_all = 3;

	static const HeaderType header_type_unknown = -1;
	static const HeaderType header_type_bad_message = -2;
	static const HeaderType header_type_bad_device = -3;
	static const HeaderType header_type_not_sysex = -4;

	 std::string parse_current();
	HeaderType parse_header();
	 std::string segment_get_next();
	 std::string segment_get_next_field();

	int segment_length;
	 std::string next_segment;

	char *buffer;
	int length;
	int position;

	 std::string parse_seg1(std::string seg1);
	 std::string parse_seg2(std::string seg2);
	 std::string parse_seg3(std::string seg3);
	 std::string parse_seg4(std::string seg4);
	 std::string parse_seg5(std::string seg5);
	 std::string parse_seg6(std::string seg6);
	 std::string parse_seg7(std::string seg7);

	 std::string current_segment;

};

#endif // SYSEX_PARSER_H
