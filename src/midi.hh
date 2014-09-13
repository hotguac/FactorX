#ifndef REFACTOR_MIDI_H
#define REFACTOR_MIDI_H

#include <string>

class MidiFactor {

public:
	MidiFactor();
	virtual ~ MidiFactor();

	//Signal handlers event buttons
	int pull_current(char *buffer, int bsize);
	int send_sysex(char *buffer, int bsize);

	 std::string get_input_ports();
	 std::string get_output_ports();

	bool set_input_port(std::string);
	bool set_output_port(std::string);

protected:
	static const char F7 = 0xF7;
	static const char F0 = 0xF0;
};

#endif // REFACTOR_MIDI_H
