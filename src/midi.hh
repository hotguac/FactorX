#ifndef REFACTOR_MIDI_H
#define REFACTOR_MIDI_H

class MidiFactor {

public:
	MidiFactor();
	virtual ~ MidiFactor();

	//Signal handlers event buttons
	int pull_current(char *buffer, int bsize);
    
    int send_sysex(char *buffer, int bsize);

protected:
	static const char F7 = 0xF7;
	static const char F0 = 0xF0;
};

extern "C" {
	int get_current_patch(char *buffer, int bsize);
	int init();
}
#endif				// REFACTOR_MIDI_H
