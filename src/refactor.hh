#ifndef GTKMM_REFACTOR_H
#define GTKMM_REFACTOR_H

#include <glibmm.h>

#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/builder.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/radiomenuitem.h>
#include <gtkmm/spinbutton.h>
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

	const static int bsize = 4096;
	char *buffer = new char[bsize];

	//Timers
	int m_timer_number;
	bool on_timeout();

	//Screen updates
	Gtk::SpinButton * pDepth;
	Gtk::SpinButton * pDmod;
	Gtk::SpinButton * pSpeed;
	Gtk::SpinButton * pSmod;
	Gtk::SpinButton * pModRate;
	Gtk::SpinButton * pIntensity;
	Gtk::SpinButton * pType;
	Gtk::SpinButton * pShape;
	Gtk::SpinButton * pModSource;
	Gtk::SpinButton * pXnob;

	Gtk::ProgressBar * pAbs_Group_Bar;
	Gtk::ProgressBar * pAbs_Depth_Bar;
	Gtk::ProgressBar * pAbs_Speed_Bar;
	Gtk::ProgressBar * pAbs_Smod_Bar;
	Gtk::ProgressBar * pAbs_ModRate_Bar;
	Gtk::ProgressBar * pAbs_Intensity_Bar;
	//Gtk::ProgressBar * pAbs_Type_Bar;
	Gtk::ProgressBar * pAbs_Shape_Bar;
	Gtk::ProgressBar * pAbs_ModSource_Bar;
	Gtk::ProgressBar * pAbs_Xnob_Bar;
	Gtk::ProgressBar * pAbs_Dmod_Bar;

	Gtk::ComboBoxText * pXnobDrob;

	Gtk::ComboBoxText * pGroup;
	Gtk::ComboBoxText * pModSourceDrop;

	void get_screen_fields();
	void update_shown();

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
	void on_input_assigned(Gtk::RadioMenuItem * choice);
	void on_output_assigned(Gtk::RadioMenuItem * choice);

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
	Gtk::ImageMenuItem * pPaste;
	Gtk::ImageMenuItem * pDelete;

	Gtk::ImageMenuItem * pAbout;

	Glib::RefPtr < Gtk::TextBuffer > pOutputBuffer;

	Gtk::TextView * pOutput;

	Gtk::Box * pTop;
	Gtk::Menu * pJack_input_menu;
	Gtk::Menu * pJack_output_menu;

	Gtk::RadioMenuItem * pOutput_none;
	Gtk::RadioMenuItem * pInput_none;

	// utility functions
	std::string getexepath();
};

#endif // GTKMM_REFACTOR_H
