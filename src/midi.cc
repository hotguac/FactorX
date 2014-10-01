#include "jack_stuff.h"
#include "midi.hh"

#include <iostream>

char *buffer;
int bsize;

MidiFactor::MidiFactor()
{
	// initialize the jack midi connection
	int result = init();
}

MidiFactor::~MidiFactor()
{
	delete buffer;
}

int MidiFactor::pull_current(char *buffer, int bsize)
{
	int result;

	result = get_current_patch(buffer, bsize);

	return result;
}

int MidiFactor::send_sysex(char *buffer, int bsize)
{
	int result;

	result = send_immediate(buffer, bsize);

	return result;
}

std::string MidiFactor::get_input_ports()
{
	std::string name;
	std::string result;

	result = "";

	int i = 0;
	name = input_port_name(i);

	while (name != "DONE") {
		if (result == "")
			result = name;
		else
			result = result + "|" + name;

		i++;
		name = input_port_name(i);
	}

	return result;
}

std::string MidiFactor::get_output_ports()
{
	std::string name;
	std::string result;

	result = "";

	int i = 0;
	name = output_port_name(i);

	while (name != "DONE") {
		if (result == "")
			result = name;
		else
			result = result + "|" + name;

		i++;
		name = output_port_name(i);
	}

	return result;
}

bool MidiFactor::set_input_port(std::string port_name)
{
	connect_to_input(&port_name[0]);
	return true;
}

bool MidiFactor::set_output_port(std::string port_name)
{
	connect_to_output(&port_name[0]);
	return true;
}
