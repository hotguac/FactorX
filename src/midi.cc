#include "jack_stuff.h"
#include "midi.hh"

#include <iostream>

char *buffer;
int bsize;

MidiFactor::MidiFactor()
{
	// initialize the jack midi connection
	int result = init();
	std::cout << input_port_name(0) << '\n';
	std::cout << output_port_name(0) << '\n';
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

	std::cout << "in midifactor send_immediate" << '\n';

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

	while (name != "DONE") 
		{
			std::cout << "found " << name << '\n';
			result = result + "|" + name;
			i++;
			name = input_port_name(i);
		}

	return result;
}

std::string MidiFactor::get_output_ports()
{
	return output_port_name(0);
}

bool set_input_port(std::string port_name)
{
	input_port(&port_name[0]);
	return true;
}

bool set_output_port(std::string port_name)
{
	output_port(&port_name[0]);
	return true;
}
