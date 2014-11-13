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

	int bsize = 4096;
	char *buffer = new char[bsize];

	//Timers
	int m_timer_number;
	bool on_timeout();

	//Screen updates
	Gtk::SpinButton * pDepth;
	Gtk::SpinButton * pAbs_Depth;
	Gtk::SpinButton * pDmod;
	Gtk::SpinButton * pAbs_Dmod;
	Gtk::SpinButton * pSpeed;
	Gtk::SpinButton * pAbs_Speed;
	Gtk::SpinButton * pSmod;
	Gtk::SpinButton * pAbs_Smod;
	Gtk::SpinButton * pModRate;
	Gtk::SpinButton * pAbs_ModRate;
	Gtk::SpinButton * pIntensity;
	Gtk::SpinButton * pAbs_Intensity;
	Gtk::SpinButton * pType;
	Gtk::SpinButton * pAbs_Type;
	Gtk::SpinButton * pShape;
	Gtk::SpinButton * pAbs_Shape;
	Gtk::SpinButton * pModSource;
	Gtk::SpinButton * pAbs_ModSource;
	Gtk::SpinButton * pXnob;
	Gtk::SpinButton * pAbs_Xnob;
	Gtk::SpinButton * pAbs_Group;

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
};

#endif // GTKMM_REFACTOR_H
