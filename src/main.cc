#include "refactor.hh"

/*
#include <gtkmm/window.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/builder.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/textview.h>

#include <glibmm.h>
*/

#include <gtkmm/application.h>
#include <iostream>

int main(int argc, char *argv[])
{

	/* 
	   creates a Gtk::Application object, stored in a RefPtr smartpointer. This is needed
	   in all gtkmm applications. The create() method for this object initializes gtkmm, 
	   and checks the arguments passed to your application on the command line, looking 
	   for standard options such as --display. It takes these from the argument list, 
	   leaving anything it does not recognize for your application to parse or ignore. 
	   This ensures that all gtkmm applications accept the same set of standard arguments.
	 */

	Glib::RefPtr < Gtk::Application > app =
	    Gtk::Application::create(argc, argv, "com.hotguac.refactor");

	Refactor refactor;
	app->run(refactor);

	return 0;
}
