#include <string>
#include <limits.h>
#include <unistd.h>

#include "refactor.hh"
#include "sysex_parser.hh"

#include <iostream>

Refactor::Refactor():
m_timer_number(0)
{
	std::string glade_file;

	set_border_width(12);

	// show the exe directory
	std::cerr << getexepath() << std::endl;

	glade_file.append(getexepath());
	glade_file.append(".glade");
	std::cerr << glade_file << std::endl;

	// load UI from glade file.  Skip the top level window container because Refactor is a top level window
	// builder = Gtk::Builder::create_from_file("reFactor.glade", "boxTop");
	builder = Gtk::Builder::create_from_file(glade_file, "boxTop");

	builder->get_widget("boxTop", pTop);
	add(*pTop);

	// Now set up the signal handlers
	attach_signal_handlers();
	attachOutput();
	populate_jack_io_menu();
	get_screen_fields();
}

Refactor::~Refactor()
{
	//delete pTop;
}

std::string Refactor::getexepath()
{
	char result[ PATH_MAX ];
	ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
	return std::string( result, (count > 0) ? count : 0 );
}


void Refactor::get_screen_fields()
{
	builder->get_widget("depth", pDepth);
	pDepth->get_adjustment()->set_lower(0);
	pDepth->get_adjustment()->set_upper(100);
	pDepth->get_adjustment()->set_value(0.0);
	pDepth->get_adjustment()->set_page_increment(1);
	pDepth->get_adjustment()->set_step_increment(0.1);
	pDepth->update();

	builder->get_widget("abs_depth_bar", pAbs_Depth_Bar);
	pAbs_Depth_Bar->set_fraction(0.1);

	builder->get_widget("dmod", pDmod);
	pDmod->get_adjustment()->set_lower(0);
	pDmod->get_adjustment()->set_upper(100);
	pDmod->get_adjustment()->set_value(0.0);
	pDmod->get_adjustment()->set_page_increment(1);
	pDmod->get_adjustment()->set_step_increment(0.1);
	pDmod->update();

	builder->get_widget("abs_dmod_bar", pAbs_Dmod_Bar);
	pAbs_Dmod_Bar->set_fraction(0.1);

	builder->get_widget("speed", pSpeed);
	pSpeed->get_adjustment()->set_lower(0);
	pSpeed->get_adjustment()->set_upper(100);
	pSpeed->get_adjustment()->set_value(0.0);
	pSpeed->get_adjustment()->set_page_increment(1);
	pSpeed->get_adjustment()->set_step_increment(0.1);
	pSpeed->update();

	builder->get_widget("abs_speed_bar", pAbs_Speed_Bar);
	pAbs_Speed_Bar->set_fraction(0.2);

	builder->get_widget("smod", pSmod);
	pSmod->get_adjustment()->set_lower(0);
	pSmod->get_adjustment()->set_upper(100);
	pSmod->get_adjustment()->set_value(0.0);
	pSmod->get_adjustment()->set_page_increment(1);
	pSmod->get_adjustment()->set_step_increment(0.1);
	pSmod->update();

	builder->get_widget("abs_smod_bar", pAbs_Smod_Bar);
	pAbs_Smod_Bar->set_fraction(0.2);

	builder->get_widget("modrate", pModRate);
	pModRate->get_adjustment()->set_lower(0);
	pModRate->get_adjustment()->set_upper(100);
	pModRate->get_adjustment()->set_value(0.0);
	pModRate->get_adjustment()->set_page_increment(1);
	pModRate->get_adjustment()->set_step_increment(0.1);
	pModRate->update();

	builder->get_widget("abs_modrate_bar", pAbs_ModRate_Bar);
	pAbs_ModRate_Bar->set_fraction(0.2);

	builder->get_widget("intensity", pIntensity);
	pIntensity->get_adjustment()->set_lower(0);
	pIntensity->get_adjustment()->set_upper(100);
	pIntensity->get_adjustment()->set_value(0.0);
	pIntensity->get_adjustment()->set_page_increment(1);
	pIntensity->get_adjustment()->set_step_increment(0.1);
	pIntensity->update();

	builder->get_widget("abs_intensity_bar", pAbs_Intensity_Bar);
	pAbs_Intensity_Bar->set_fraction(0.2);

	builder->get_widget("type", pType);
	pType->get_adjustment()->set_lower(0);
	pType->get_adjustment()->set_upper(100);
	pType->get_adjustment()->set_value(0.0);
	pType->get_adjustment()->set_page_increment(1);
	pType->get_adjustment()->set_step_increment(0.1);
	pType->update();

	//builder->get_widget("abs_type_bar", pAbs_Type_Bar);
	//pAbs_Type_Bar->set_fraction(0.2);

	builder->get_widget("shape", pShape);
	pShape->get_adjustment()->set_lower(0);
	pShape->get_adjustment()->set_upper(200);
	pShape->get_adjustment()->set_value(0.0);
	pShape->get_adjustment()->set_page_increment(1);
	pShape->get_adjustment()->set_step_increment(0.1);
	pShape->update();

	builder->get_widget("abs_shape_bar", pAbs_Shape_Bar);
	pAbs_Shape_Bar->set_fraction(0.2);

	builder->get_widget("xnob", pXnob);
	pXnob->get_adjustment()->set_lower(0);
	pXnob->get_adjustment()->set_upper(200);
	pXnob->get_adjustment()->set_value(0.0);
	pXnob->get_adjustment()->set_page_increment(1);
	pXnob->get_adjustment()->set_step_increment(0.1);
	pXnob->update();

	builder->get_widget("abs_xnob_bar", pAbs_Xnob_Bar);
	pAbs_Xnob_Bar->set_fraction(0.2);

	builder->get_widget("mod_source", pModSource);
	pModSource->get_adjustment()->set_lower(0);
	pModSource->get_adjustment()->set_upper(100);
	pModSource->get_adjustment()->set_value(0.0);
	pModSource->get_adjustment()->set_page_increment(1);
	pModSource->get_adjustment()->set_step_increment(0.1);
	pModSource->update();

	builder->get_widget("abs_mod_source_bar", pAbs_ModSource_Bar);
	pAbs_ModSource_Bar->set_fraction(0.2);

	builder->get_widget("abs_group_bar", pAbs_Group_Bar);
	pAbs_Group_Bar->set_fraction(0.30);

	builder->get_widget("group", pGroup);
}

void Refactor::update_shown()
{
	pDepth->get_adjustment()->set_value(parser.current.depth);
	pDepth->update();

	pAbs_Depth_Bar->set_fraction(parser.current.abs_depth/2048.0);

	pDmod->get_adjustment()->set_value(parser.current.dmod);
	pDmod->update();

	pAbs_Dmod_Bar->set_fraction(parser.current.abs_dmod/2048.0);

	pSpeed->get_adjustment()->set_value(parser.current.speed);
	pSpeed->update();

	pAbs_Speed_Bar->set_fraction(parser.current.abs_speed/2048.0);

	pSmod->get_adjustment()->set_value(parser.current.smod);
	pSmod->update();

	pAbs_Smod_Bar->set_fraction(parser.current.abs_smod/2048.0);

	pModRate->get_adjustment()->set_value(parser.current.mod_rate);
	pModRate->update();

	pAbs_ModRate_Bar->set_fraction(parser.current.abs_mod_rate/2048.0);

	pIntensity->get_adjustment()->set_value(parser.current.intensity);
	pIntensity->update();

	pAbs_Intensity_Bar->set_fraction(parser.current.abs_intensity/2048.0);

	pType->get_adjustment()->set_value(parser.current.type);
	pType->update();

	//pAbs_Type_Bar->set_fraction(parser.current.abs_type/2048.0);

	pShape->get_adjustment()->set_value(parser.current.shape);
	pShape->update();

	pAbs_Shape_Bar->set_fraction(parser.current.abs_shape/2048.0);

	pXnob->get_adjustment()->set_value(parser.current.xnob);
	pXnob->update();

	pAbs_Xnob_Bar->set_fraction(parser.current.abs_xnob/2048.0);

	pModSource->get_adjustment()->set_value(parser.current.mod_source);
	pModSource->update();

	pAbs_ModSource_Bar->set_fraction(parser.current.abs_mod_source/2048.0);

	pAbs_Group_Bar->set_fraction(parser.current.abs_group/1024.0);
	pGroup->set_active(parser.current.group);
}

bool Refactor::populate_jack_io_menu()
{
	std::string input_names;
	std::string output_names;
	std::string delimiter = "|";
	size_t pos = 0;
	std::string token;
	int escape;
	Gtk::MenuItem * next;
	Gtk::Menu * jack_output;
	Gtk::Menu * jack_input;

	Gtk::RadioMenuItem * input_none;
	Gtk::RadioMenuItem * output_none;
	Gtk::RadioMenuItem::Group input_group;
	Gtk::RadioMenuItem::Group output_group;

	input_names = midiFactor.get_input_ports();
	output_names = midiFactor.get_output_ports();

	builder->get_widget("jack_inputmenu", jack_input);
	builder->get_widget("jack_outputmenu", jack_output);

	builder->get_widget("jack_input_none", input_none);
	builder->get_widget("jack_output_none", output_none);

	input_none->set_group(input_group);
	output_none->set_group(output_group);

	output_none->signal_toggled().connect(sigc::bind <
					      Gtk::RadioMenuItem *
					      >(sigc::mem_fun(*this,
							      &Refactor::
							      on_output_assigned),
						output_none));
	input_none->signal_toggled().connect(sigc::bind <
					     Gtk::RadioMenuItem *
					     >(sigc::
					       mem_fun(*this,
						       &Refactor::
						       on_input_assigned),
					       input_none));

	pos = input_names.find(delimiter);

	escape = 0;
	while (((pos = input_names.find(delimiter)) != std::string::npos)
	       && escape < 3) {
		escape++;
		token = input_names.substr(0, pos);
		std::cout << token << std::endl;
		input_names.erase(0, pos + delimiter.length());

		Gtk::RadioMenuItem * next =
		    Gtk::manage(new Gtk::
				RadioMenuItem(output_group, token, true));
		next->signal_toggled().connect(sigc::bind <
					       Gtk::RadioMenuItem *
					       >(sigc::
						 mem_fun(*this,
							 &Refactor::
							 on_output_assigned),
						 next));
		jack_output->add(*next);
		next->show();
	}

	pos = output_names.find(delimiter);

	escape = 0;
	while (((pos = output_names.find(delimiter)) != std::string::npos)
	       && escape < 3) {
		escape++;
		token = output_names.substr(0, pos);
		std::cout << token << std::endl;
		output_names.erase(0, pos + delimiter.length());

		Gtk::RadioMenuItem * next =
		    Gtk::manage(new Gtk::
				RadioMenuItem(input_group, token, true));
		next->signal_toggled().connect(sigc::bind <
					       Gtk::RadioMenuItem *
					       >(sigc::
						 mem_fun(*this,
							 &Refactor::
							 on_input_assigned),
						 next));
		jack_input->add(*next);
		next->show();
	}

	return true;
}

void Refactor::on_output_assigned(Gtk::RadioMenuItem * choice)
{
	//std::cerr << "on_output_assigned()" << std::endl;
	midiFactor.set_output_port(choice->get_label());
}

void Refactor::on_input_assigned(Gtk::RadioMenuItem * choice)
{
	//std::cerr << "on_input_assigned()" << std::endl;
	midiFactor.set_input_port(choice->get_label());
}

bool Refactor::attach_signal_handlers()
{
	// Pull Current Patch Button
	builder->get_widget("evbPullCurrentPatch", pPullCurrentPatch);
	if (pPullCurrentPatch) {
		pPullCurrentPatch->signal_button_press_event().
		    connect(sigc::mem_fun(*this, &Refactor::on_pull_current));
	}

	// Send Current Patch Button
	builder->get_widget("evbPushCurrentPatch", pPushCurrentPatch);
	if (pPushCurrentPatch) {
		pPushCurrentPatch->signal_button_press_event().
		    connect(sigc::mem_fun(*this, &Refactor::on_push_current));
	}

	// Quit Menu Option
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

bool Refactor::on_push_current(GdkEventButton * ev)
{
	int result;

	result = midiFactor.send_sysex(buffer, 6);
}

bool Refactor::on_pull_current(GdkEventButton * ev)
{
	//int bsize = 4096;
	//char *buffer = new char[bsize];
	//std::string s;

	int result;
	int offset;

	buffer[0] = 0xF0;	// start of sysex message
	buffer[1] = 0x1C;	// EVENTIDE
	buffer[2] = 0x70;	// H4000 formats
	buffer[3] = 0;		// any matching device
	buffer[4] = 0x4e;	// SYSEXC_TJ_PROGRAM_WANT
	buffer[5] = 0xF7;	// end of sysex message

	result = midiFactor.send_sysex(buffer, 6);

	sigc::slot < bool > my_slot =
	    sigc::mem_fun(*this, &Refactor::on_timeout);

	Glib::signal_timeout().connect(my_slot, 500);

	return 0;
}

bool Refactor::on_timeout()
{
	std::string s;

	int result;
	int offset;

	result = midiFactor.pull_current(buffer, bsize);

	offset = 0;
	int i;
	// calculate the offset of the second sysex message
	// until I figure out how to not get the echoed request
	for (i = 1; i < result; ++i) {
		if (buffer[i] == -16) {
			offset = i;
			//std::cout << "found F0" << '\n';
		}
	}

	s = parser.parse_message(buffer + offset, result - offset);

	if (s != "Bad message") {
		pOutputBuffer->insert_at_cursor(s);
		// update screen controls here
		update_shown();
		return false;
	} else {
		return true;
	}

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
