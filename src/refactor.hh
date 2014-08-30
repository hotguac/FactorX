#ifndef GTKMM_REFACTOR_H
#define GTKMM_REFACTOR_H

#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/builder.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/textview.h>
#include <gtkmm/textbuffer.h>

#include <glibmm.h>

#include <iostream>

#include "midi.hh"

class Refactor:public
    Gtk::Window
{

  public:
    Refactor ();
    virtual ~
    Refactor ();

  protected:
    Glib::RefPtr <
	Gtk::Builder >
	builder;
    MidiFactor midiFactor;

    Parser parser;

    //Signal handlers event buttons
    bool
    on_pull_current (GdkEventButton * ev);
    bool
    on_push_current (GdkEventButton * ev);
    bool
    on_pull_all (GdkEventButton * ev);
    bool
    on_push_all (GdkEventButton * ev);

    bool
    attach_signal_handlers ();

    bool 
    attachOutput ();

    //
    // Signal Handlers for the Menu Items
    //
    bool
    on_open_clicked (GdkEventButton * ev);
    bool
    on_new_clicked (GdkEventButton * ev);
    bool
    on_save_clicked (GdkEventButton * ev);
    bool
    on_saveas_clicked (GdkEventButton * ev);
    bool
    on_quit_clicked (GdkEventButton * ev);
    bool
    on_copy_clicked (GdkEventButton * ev);
    bool
    on_cut_clicked (GdkEventButton * ev);
    bool
    on_paste_clicked (GdkEventButton * ev);
    bool
    on_delete_clicked (GdkEventButton * ev);
    bool
    on_about_clicked (GdkEventButton * ev);

    //Member widgets:
    Gtk::Button *
	pButton;

    // copied over
    Gtk::Statusbar *
	pStatusBar;

    Gtk::EventBox *
	pPullCurrentPatch;
    Gtk::EventBox *
	pPushCurrentPatch;
    Gtk::EventBox *
	pPullAllPatches;
    Gtk::EventBox *
	pPushAllPatches;

    Gtk::ImageMenuItem *
	pNew;
    Gtk::ImageMenuItem *
	pOpen;
    Gtk::ImageMenuItem *
	pSave;
    Gtk::ImageMenuItem *
	pSaveAs;
    Gtk::ImageMenuItem *
	pQuit;

    Gtk::ImageMenuItem *
	pCopy;
    Gtk::ImageMenuItem *
	pCut;
    Gtk::ImageMenuItem *
	pCaste;
    Gtk::ImageMenuItem *
	pCelete;

    Gtk::ImageMenuItem *
	pAbout;

    Glib::RefPtr <
    Gtk::TextBuffer > 
	pOutputBuffer;

    Gtk::TextView *
	pOutput;

    Gtk::Box *
	pTop;
};

extern
    "C"
{
    int
    get_current_patch (char* buffer, int bsize);

    int
    init ();
}

#endif				// GTKMM_REFACTOR_H
