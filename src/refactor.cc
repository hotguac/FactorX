#include "refactor.hh"
#include "sysex.hh"

#include <iostream>
#include <glibmm/ustring.h>
#include <sstream>
#include <string>

using std::cout;

Refactor::Refactor ()
{
    std::cout << "**\treFactor constructor" << std::endl;

    set_border_width (12);

    // load UI from glade file.  Skip the top level window container because Refactor is a top level window
    builder = Gtk::Builder::create_from_file ("basic.glade", "boxTop");

    builder->get_widget ("boxTop", pTop);
    add (*pTop);

    // Now set up the signal handlers
    attach_signal_handlers ();
    attachOutput ();
}

Refactor::~Refactor ()
{
    delete pTop;
}

bool Refactor::attach_signal_handlers ()
{
    builder->get_widget ("evbPullCurrentPatch", pPullCurrentPatch);
    if (pPullCurrentPatch)
      {
	  pPullCurrentPatch->signal_button_press_event ().connect (sigc::mem_fun (*this, &Refactor::on_pull_current));
      }

    builder->get_widget ("mnuQuit", pQuit);
    if (pQuit)
      {
	  pQuit->signal_button_press_event ().connect (sigc::mem_fun (*this, &Refactor::on_quit_clicked));
      }

    builder->get_widget ("mnuOpen", pOpen);
    if (pOpen)
      {
	  pOpen->signal_button_press_event ().connect (sigc::mem_fun (*this, &Refactor::on_open_clicked));
      }

    return 0;
}

bool Refactor::attachOutput ()
{
    builder->get_widget ("txtOutput", pOutput);

    pOutputBuffer = Gtk::TextBuffer::create();
    pOutput->set_buffer(pOutputBuffer);

}

bool Refactor::on_pull_current (GdkEventButton * ev)
{
    int bsize = 4096;
    char* buffer = new char[bsize];

    int result;
    result = midiFactor.pull_current (buffer, bsize);

    Glib::ustring text;
    std::string s;
    std::stringstream strStream (std::stringstream::in | std::stringstream::out);

    int i;
    short asc;
    bool started;

    started = true;

    if (result > 5)
	{
	    if (buffer[0] != -16)
		{
		    strStream << "Expected a SYSEX message" << '\n';
		}
	    else if ((buffer[1] != 28) || (buffer[2] != 'p') || (buffer[3] != 1))			
		{
		    strStream << "Unknown Device" << '\n';
		}
		
	    switch (buffer[4])
		{
		case 'I':
		    strStream << "Presets" << '\n';
		    break;
		case 'O':
		    strStream << "Current" << '\n';
		    break;
		case 'M':
		    strStream << "System" << '\n';
		    break;
		case 'Q':
		    strStream << "All" << '\n';
		    break;
		}
	}

    for (i = 5; i < result; i++) 
	{

	    if (isprint(buffer[i]))
		{
		    if (!started) 
			{
			    strStream << '\n' << "[" << i << "]" << '\t' << "'";
			    started = true;
			}
		    
		    strStream << buffer[i];			    
		} else
		{
		    if (started)
			{
			    strStream << "'";
			    started = false;
			    strStream << '\n' << "[" << i << "]" << '\t';
			}
		    
		    asc = buffer[i];
		    strStream << "<" << asc << ">";
		}
	}

    if (started)
	{
	    strStream << "'";
	}

    strStream << '\n';
    s = strStream.str();

    std::cout << '\n' << "strStream = " << '\n' << s << '\n';
    pOutputBuffer->set_text (s);

    return 0;
}

bool Refactor::on_open_clicked (GdkEventButton * ev)
{
    std::cerr << "on_open_clicked()" << std::endl;
}

//
// Signal Handlers for the Menu Items
//
bool Refactor::on_new_clicked (GdkEventButton * ev)
{
    std::cerr << "on_new_clicked()" << std::endl;
}

bool Refactor::on_save_clicked (GdkEventButton * ev)
{
    std::cerr << "on_save_clicked()" << std::endl;
}

bool Refactor::on_saveas_clicked (GdkEventButton * ev)
{
    std::cerr << "on_saveas_clicked()" << std::endl;
}

bool Refactor::on_quit_clicked (GdkEventButton * ev)
{
    std::cerr << "on_button_clicked()" << std::endl;

    hide ();			//hide() will cause main::run() to end.

    return 0;
}

bool Refactor::on_copy_clicked (GdkEventButton * ev)
{
    std::cerr << "on_copy_clicked()" << std::endl;
}

bool Refactor::on_cut_clicked (GdkEventButton * ev)
{
    std::cerr << "on_cut_clicked()" << std::endl;
}

bool Refactor::on_paste_clicked (GdkEventButton * ev)
{
    std::cerr << "on_paste_clicked()" << std::endl;
}

bool Refactor::on_delete_clicked (GdkEventButton * ev)
{
    std::cerr << "on_delete_clicked()" << std::endl;
}

bool Refactor::on_about_clicked (GdkEventButton * ev)
{
    std::cerr << "on_about_clicked()" << std::endl;
}
