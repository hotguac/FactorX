#ifndef SYSEX_PARSER_H
#define SYSEX_PARSER_H

#include <string>

typedef signed short int HeaderType;
static const char BUFFER_END = 0;
static const char SEGMENT_END = 13;
/*
next segment = [5] 4 3
next segment =  4 1b80 0 3ff0 cc9 0 0 4095 4095 0 4095 0
next segment =  0 0 0 0 0 0 0 0 1476 7fe0 63be 0 0 0 0 0 0 0 0 0
next segment =  0 0 0 0
next segment =  21.29 0 8 6 0.68 0 13.77 0.57 0.45 31.68 65000 65000
next segment = C_1e2f
next segment = Talking Back
next segment = [5] 5 3
next segment =  5 0 0 3ff0 cc9 0 3ff0 4095 4095 0 7fe0 0
next segment =  0 0 0 0 0 0 0 0 1476 7fe0 63be 0 0 0 0 0 0 0 0 0
next segment =  0 0 0 0
next segment =  21.29 0 8 6 0.68 0 13.77 0.57 0.45 31.68 65000 65000
next segment = C_81eb
next segment = Talking Back
 */

class HeaderSeg {
public:
	int unknown1;
	int unknown2;
	int unknown3;
};

class FineSeg {
public:
	int x;
};

class Parser {
public:
	Parser();
	virtual ~ Parser();

	std::string parse_message(char *buffer, int length);

	struct Patch {
		std::string patch_name;
		int group;
		double type;
		double intensity;
		double depth;
		double speed;
		double shape;
		double xnob;

		double dmod;
		double smod;
		double mod_rate;
		double mod_source;

		int abs_group;
		int abs_type;
		int abs_intensity;
		int abs_depth;
		int abs_speed;
		int abs_shape;
		int abs_xnob;

		int abs_dmod;
		int abs_smod;
		int abs_mod_rate;
		int abs_mod_source;
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

	int convert_abs(std::string value);

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
