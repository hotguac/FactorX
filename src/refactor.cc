#include "refactor.hh"
#include "sysex_parser.hh"

#include <iostream>
#include <glibmm/ustring.h>
#include <sstream>
#include <string>
#include <gtkmm/radiomenuitem.h>

using std::cout;

Refactor::Refactor()
{
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
	std::string delimiter = "|";
	size_t pos = 0;
	std::string token;

	builder->get_widget("jack_output_none", pOutput_none);
	builder->get_widget("out1", pOutput_1);
	builder->get_widget("out2", pOutput_2);
	builder->get_widget("out3", pOutput_3);
	builder->get_widget("out4", pOutput_4);
	builder->get_widget("out5", pOutput_5);

	input_names = midiFactor.get_input_ports();

	if ((pos = input_names.find(delimiter)) != std::string::npos) 
		{
		token = input_names.substr(0, pos);
		std::cout << token << std::endl;
		input_names.erase(0, pos + delimiter.length());
		
		pOutput_1->set_label(token);
		} 
	else if (input_names.length() > 0)
		{
			pOutput_1->set_label(input_names);
		}
	else
		{
			pOutput_1->hide();
		}

	if ((pos = input_names.find(delimiter)) != std::string::npos) {
		token = input_names.substr(0, pos);
		std::cout << token << std::endl;
		input_names.erase(0, pos + delimiter.length());
		
		pOutput_2->set_label(token);
	}
	else if (input_names.length() > 0)
		{
			pOutput_2->set_label(input_names);
		}
	else 
		{
			pOutput_2->hide();
		}

	if ((pos = input_names.find(delimiter)) != std::string::npos) {
		token = input_names.substr(0, pos);
		std::cout << token << std::endl;
		input_names.erase(0, pos + delimiter.length());
		
		pOutput_3->set_label(token);
	}
	else if (input_names.length() > 0)
		{
			pOutput_3->set_label(input_names);
		}
	else
		{
			pOutput_3->hide();
		}

	pOutput_4->hide();
	pOutput_5->hide();

	if (pOutput_none)
		{
		pOutput_none->signal_button_press_event().connect(sigc::mem_fun(*this,
									 &Refactor::on_output_none_assigned));
		}

	if (pOutput_1) {
		pOutput_1->signal_button_press_event().connect(sigc::mem_fun(*this,
									 &Refactor::on_output1_assigned));
	}

	if (pOutput_2) {
		pOutput_2->signal_button_press_event().connect(sigc::mem_fun(*this,
									 &Refactor::on_output2_assigned));
	}

	if (pOutput_3) {
		pOutput_3->signal_button_press_event().connect(sigc::mem_fun(*this,
									 &Refactor::on_output3_assigned));
	}

	if (pOutput_4) {
		pOutput_4->signal_button_press_event().connect(sigc::mem_fun(*this,
									 &Refactor::on_output4_assigned));
	}

	if (pOutput_5) {
		pOutput_5->signal_button_press_event().connect(sigc::mem_fun(*this,
									 &Refactor::on_output5_assigned));
	}



	// now do 

	builder->get_widget("jack_inputmenu", pJack_input_menu);

	output_names = midiFactor.get_output_ports();

	// now add the list to the jack_outputmenu and jack_inputmenu
	builder->get_widget("jack_input_none", pInput_none);
	builder->get_widget("in1", pInput_1);
	builder->get_widget("in2", pInput_2);
	builder->get_widget("in3", pInput_3);
	builder->get_widget("in4", pInput_4);
	builder->get_widget("in5", pInput_5);

	if ((pos = output_names.find(delimiter)) != std::string::npos) 
		{
		token = output_names.substr(0, pos);
		std::cout << token << std::endl;
		output_names.erase(0, pos + delimiter.length());
		
		pInput_1->set_label(token);
		} 
	else if (input_names.length() > 0)
		{
			pInput_1->set_label(output_names);
		}
	else
		{
			pInput_1->hide();
		}

	if ((pos = output_names.find(delimiter)) != std::string::npos) 
		{
		token = output_names.substr(0, pos);
		std::cout << token << std::endl;
		output_names.erase(0, pos + delimiter.length());
		
		pInput_2->set_label(token);
		} 
	else if (output_names.length() > 0)
		{
			pInput_2->set_label(output_names);
		}
	else
		{
			pInput_2->hide();
		}

	if ((pos = output_names.find(delimiter)) != std::string::npos) 
		{
		token = output_names.substr(0, pos);
		std::cout << token << std::endl;
		output_names.erase(0, pos + delimiter.length());
		
		pInput_3->set_label(token);
		} 
	else if (output_names.length() > 0)
		{
			pInput_3->set_label(output_names);
		}
	else
		{
			pInput_3->hide();
		}


	pInput_4->hide();
	pInput_5->hide();

	if (pInput_none)
		{
		pInput_none->signal_button_press_event().connect(sigc::mem_fun(*this,
									 &Refactor::on_input_none_assigned));
		}

	if (pInput_1) {
		pInput_1->signal_button_press_event().connect(sigc::mem_fun(*this,
									 &Refactor::on_input1_assigned));
	}

	if (pInput_2) {
		pInput_2->signal_button_press_event().connect(sigc::mem_fun(*this,
									 &Refactor::on_input2_assigned));
	}

	if (pInput_3) {
		pInput_3->signal_button_press_event().connect(sigc::mem_fun(*this,
									 &Refactor::on_input3_assigned));
	}

	if (pInput_4) {
		pInput_4->signal_button_press_event().connect(sigc::mem_fun(*this,
									 &Refactor::on_input4_assigned));
	}

	if (pInput_5) {
		pInput_5->signal_button_press_event().connect(sigc::mem_fun(*this,
									 &Refactor::on_input5_assigned));
	}

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

	result = midiFactor.send_sysex(buffer, 6);

	// need to wait here for the response
	// change to a separate thread with a callback

	sleep(5);

	result = midiFactor.pull_current(buffer, bsize);

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

bool Refactor::on_input_none_assigned(GdkEventButton * ev)
{
	std::cerr << "on_input_none_assigned()" << std::endl;
	midiFactor.set_input_port(pInput_none->get_label());
}

bool Refactor::on_input1_assigned(GdkEventButton * ev)
{
	std::cerr << "on_input1_assigned()" << std::endl;
	midiFactor.set_input_port(pInput_1->get_label());
}

bool Refactor::on_input2_assigned(GdkEventButton * ev)
{
	std::cerr << "on_input2_assigned()" << std::endl;
	midiFactor.set_input_port(pInput_2->get_label());
}

bool Refactor::on_input3_assigned(GdkEventButton * ev)
{
	std::cerr << "on_input3_assigned()" << std::endl;
	midiFactor.set_input_port(pInput_3->get_label());
}

bool Refactor::on_input4_assigned(GdkEventButton * ev)
{
	std::cerr << "on_input4_assigned()" << std::endl;
	midiFactor.set_input_port(pInput_4->get_label());
}

bool Refactor::on_input5_assigned(GdkEventButton * ev)
{
	std::cerr << "on_input5_assigned()" << std::endl;
	midiFactor.set_input_port(pInput_5->get_label());
}

bool Refactor::on_output_none_assigned(GdkEventButton * ev)
{
	std::cerr << "on_output_none_assigned()" << std::endl;
	midiFactor.set_output_port(pOutput_none->get_label());
}

bool Refactor::on_output1_assigned(GdkEventButton * ev)
{
	std::cerr << "on_output1_assigned()" << std::endl;
	midiFactor.set_output_port(pOutput_1->get_label());
}

bool Refactor::on_output2_assigned(GdkEventButton * ev)
{
	std::cerr << "on_output2_assigned()" << std::endl;
	midiFactor.set_output_port(pOutput_2->get_label());
}

bool Refactor::on_output3_assigned(GdkEventButton * ev)
{
	std::cerr << "on_output3_assigned()" << std::endl;
	midiFactor.set_output_port(pOutput_3->get_label());
}

bool Refactor::on_output4_assigned(GdkEventButton * ev)
{
	std::cerr << "on_output4_assigned()" << std::endl;
	midiFactor.set_output_port(pOutput_4->get_label());
}

bool Refactor::on_output5_assigned(GdkEventButton * ev)
{
	std::cerr << "on_output5_assigned()" << std::endl;
	midiFactor.set_output_port(pOutput_5->get_label());
}

