#ifndef GTKMM_REFACTOR_H
#define GTKMM_REFACTOR_H

#include <glibmm.h>

#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/builder.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/radiomenuitem.h>
#include <gtkmm/textview.h>
#include <gtkmm/textbuffer.h>

#include "midi.hh"
#include "sysex_parser.hh"

class Refactor:public Gtk::Window {

public:
	Refactor();
	virtual ~ Refactor();

protected:
	Glib::RefPtr < Gtk::Builder > builder;
	MidiFactor midiFactor;
	Parser parser;

	//Signal handlers event buttons
	bool on_pull_current(GdkEventButton * ev);
	bool on_push_current(GdkEventButton * ev);
	bool on_pull_all(GdkEventButton * ev);
	bool on_push_all(GdkEventButton * ev);

	bool attach_signal_handlers();
	bool attachOutput();
	bool populate_jack_io_menu();

	//
	// Signal Handlers for the Menu Items
	//
	bool on_open_clicked(GdkEventButton * ev);
	bool on_new_clicked(GdkEventButton * ev);
	bool on_save_clicked(GdkEventButton * ev);
	bool on_saveas_clicked(GdkEventButton * ev);
	bool on_quit_clicked(GdkEventButton * ev);
	bool on_copy_clicked(GdkEventButton * ev);
	bool on_cut_clicked(GdkEventButton * ev);
	bool on_paste_clicked(GdkEventButton * ev);
	bool on_delete_clicked(GdkEventButton * ev);
	bool on_about_clicked(GdkEventButton * ev);

	// signal handlers for input and output
	bool on_input_none_assigned(GdkEventButton * ev);
	bool on_input1_assigned(GdkEventButton * ev);
	bool on_input2_assigned(GdkEventButton * ev);
	bool on_input3_assigned(GdkEventButton * ev);
	bool on_input4_assigned(GdkEventButton * ev);
	bool on_input5_assigned(GdkEventButton * ev);

	bool on_output_none_assigned(GdkEventButton * ev);
	bool on_output1_assigned(GdkEventButton * ev);
	bool on_output2_assigned(GdkEventButton * ev);
	bool on_output3_assigned(GdkEventButton * ev);
	bool on_output4_assigned(GdkEventButton * ev);
	bool on_output5_assigned(GdkEventButton * ev);

	//Member widgets:
	Gtk::Button * pButton;

	// copied over
	Gtk::Statusbar * pStatusBar;

	Gtk::EventBox * pPullCurrentPatch;
	Gtk::EventBox * pPushCurrentPatch;
	Gtk::EventBox * pPullAllPatches;
	Gtk::EventBox * pPushAllPatches;

	Gtk::ImageMenuItem * pNew;
	Gtk::ImageMenuItem * pOpen;
	Gtk::ImageMenuItem * pSave;
	Gtk::ImageMenuItem * pSaveAs;
	Gtk::ImageMenuItem * pQuit;

	Gtk::ImageMenuItem * pCopy;
	Gtk::ImageMenuItem * pCut;
	Gtk::ImageMenuItem * pCaste;
	Gtk::ImageMenuItem * pCelete;

	Gtk::ImageMenuItem * pAbout;

	Glib::RefPtr < Gtk::TextBuffer > pOutputBuffer;

	Gtk::TextView * pOutput;

	Gtk::Box * pTop;
	Gtk::Menu * pJack_input_menu;
	Gtk::Menu * pJack_output_menu;

	Gtk::RadioMenuItem * pOutput_none;
	Gtk::RadioMenuItem * pOutput_1;
	Gtk::RadioMenuItem * pOutput_2;
	Gtk::RadioMenuItem * pOutput_3;
	Gtk::RadioMenuItem * pOutput_4;
	Gtk::RadioMenuItem * pOutput_5;

	Gtk::RadioMenuItem * pInput_none;
	Gtk::RadioMenuItem * pInput_1;
	Gtk::RadioMenuItem * pInput_2;
	Gtk::RadioMenuItem * pInput_3;
	Gtk::RadioMenuItem * pInput_4;
	Gtk::RadioMenuItem * pInput_5;
	
};

#endif // GTKMM_REFACTOR_H
