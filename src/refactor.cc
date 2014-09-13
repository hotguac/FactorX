#include "refactor.hh"
#include "sysex_parser.hh"

#include <iostream>
#include <glibmm/ustring.h>
#include <sstream>
#include <string>

using std::cout;

Refactor::Refactor()
{
	std::cout << "**\treFactor constructor" << std::endl;

	set_border_width(12);

	// load UI from glade file.  Skip the top level window container because Refactor is a top level window
	builder = Gtk::Builder::create_from_file("reFactor.glade", "boxTop");

	builder->get_widget("boxTop", pTop);
	add(*pTop);

	// Now set up the signal handlers
	attach_signal_handlers();
	attachOutput();
	populate_jack_io_menu();
}

Refactor::~Refactor()
{
	//delete pTop;
}

bool Refactor::populate_jack_io_menu()
{
 	std::string input_names;
	std::string output_names;

	input_names = midiFactor.get_input_ports();
	std::cout << input_names << '\n';

	output_names = midiFactor.get_output_ports();
	std::cout << output_names << '\n';

	return true;
}

bool Refactor::attach_signal_handlers()
{
	builder->get_widget("evbPullCurrentPatch", pPullCurrentPatch);
	if (pPullCurrentPatch) {
		pPullCurrentPatch->signal_button_press_event().
		    connect(sigc::mem_fun(*this, &Refactor::on_pull_current));
	}

	builder->get_widget("mnuQuit", pQuit);
	if (pQuit) {
		pQuit->signal_button_press_event().connect(sigc::mem_fun(*this,
									 &Refactor::on_quit_clicked));
	}

	builder->get_widget("mnuOpen", pOpen);
	if (pOpen) {
		pOpen->signal_button_press_event().connect(sigc::mem_fun(*this,
									 &Refactor::on_open_clicked));
	}

	return 0;
}

bool Refactor::attachOutput()
{
	builder->get_widget("txtOutput", pOutput);

	pOutputBuffer = Gtk::TextBuffer::create();
	pOutput->set_buffer(pOutputBuffer);
}

bool Refactor::on_pull_current(GdkEventButton * ev)
{
	int bsize = 4096;
	char *buffer = new char[bsize];
	std::string s;

	int result;
	int offset;

	buffer[0] = 0xF0;	// start of sysex message
	buffer[1] = 0x1C;	// EVENTIDE
	buffer[2] = 0x70;	// H4000 formats
	buffer[3] = 0;		// any matching device
	buffer[4] = 0x4e;	// SYSEXC_TJ_PROGRAM_WANT
	buffer[5] = 0xF7;	// end of sysex message

	cout << "calling midifactor send_immediate" << '\n';

	result = midiFactor.send_sysex(buffer, 6);

	// need to wait here for the response
	// change to a separate thread with a callback

	sleep(5);

	result = midiFactor.pull_current(buffer, bsize);
	cout << "back from pull_current with result = " << result << '\n';

	offset = 0;
	int i;
	// calculate the offset of the second sysex message
	// until I figure out how to not get the echoed request
	for (i = 1; i < result; ++i) {
		if (buffer[i] == -16) {
			offset = i;
			std::cout << "found F0" << '\n';
		}
	}

	std::cout << "offset = " << offset << '\n';

	s = parser.parse_message(buffer + offset, result - offset);

	pOutputBuffer->insert_at_cursor(s);

	return 0;
}

bool Refactor::on_open_clicked(GdkEventButton * ev)
{
	std::cerr << "on_open_clicked()" << std::endl;
}

//
// Signal Handlers for the Menu Items
//
bool Refactor::on_new_clicked(GdkEventButton * ev)
{
	std::cerr << "on_new_clicked()" << std::endl;
}

bool Refactor::on_save_clicked(GdkEventButton * ev)
{
	std::cerr << "on_save_clicked()" << std::endl;
}

bool Refactor::on_saveas_clicked(GdkEventButton * ev)
{
	std::cerr << "on_saveas_clicked()" << std::endl;
}

bool Refactor::on_quit_clicked(GdkEventButton * ev)
{
	hide();			//hide will cause main::run() to end.

	return 0;
}

bool Refactor::on_copy_clicked(GdkEventButton * ev)
{
	std::cerr << "on_copy_clicked()" << std::endl;
}

bool Refactor::on_cut_clicked(GdkEventButton * ev)
{
	std::cerr << "on_cut_clicked()" << std::endl;
}

bool Refactor::on_paste_clicked(GdkEventButton * ev)
{
	std::cerr << "on_paste_clicked()" << std::endl;
}

bool Refactor::on_delete_clicked(GdkEventButton * ev)
{
	std::cerr << "on_delete_clicked()" << std::endl;
}

bool Refactor::on_about_clicked(GdkEventButton * ev)
{
	std::cerr << "on_about_clicked()" << std::endl;
}
