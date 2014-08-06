#include "refactor.hh"
#include <iostream>

Refactor::Refactor()
{
  std::cout << "**\treFactor constructor" << std::endl;
    
  set_border_width(12);

  // load UI from glade file.  Skip the top level window container because Refactor is a top level window
  builder = Gtk::Builder::create_from_file("basic.glade", "boxTop");
  
  builder->get_widget("boxTop",pTop);
  add(*pTop);

  // Now set up the signal handlers
  attach_signal_handlers();

}

Refactor::~Refactor()
{
  delete pTop;
}

bool
Refactor::attach_signal_handlers()
{
  builder->get_widget("evbPullCurrentPatch",pPullCurrentPatch);
  if (pPullCurrentPatch)
    {
      pPullCurrentPatch->signal_button_press_event().connect( sigc::mem_fun(*this, &Refactor::on_pull_current) );
    }

  builder->get_widget("mnuQuit",pQuit);
  if (pQuit)
    {
      pQuit->signal_button_press_event().connect( sigc::mem_fun(*this, &Refactor::on_quit_clicked) );
    }

  builder->get_widget("mnuOpen",pOpen);
  if (pOpen)
    {
      pOpen->signal_button_press_event().connect( sigc::mem_fun(*this, &Refactor::on_open_clicked) );
    }

  builder->get_widget("mnuQuit",pQuit);
  if (pQuit)
    {
      pQuit->signal_button_press_event().connect( sigc::mem_fun(*this, &Refactor::on_quit_clicked) );
    }

  return 0;
}

bool 
Refactor::on_pull_current(GdkEventButton *ev)
{
  // int result = old_main("test.wav", 4, 16);
  int result = get_current_patch();

  return result;
}

bool 
Refactor::on_open_clicked(GdkEventButton *ev)
{
  int result = init();
  std::cerr << "on_open_clicked()" << std::endl;
}

//
// Signal Handlers for the Menu Items
//
bool 
Refactor::on_new_clicked(GdkEventButton *ev)
{
  std::cerr << "on_new_clicked()" << std::endl;
}


bool 
Refactor::on_save_clicked(GdkEventButton *ev)
{
  std::cerr << "on_save_clicked()" << std::endl;
}

bool 
Refactor::on_saveas_clicked(GdkEventButton *ev)
{
  std::cerr << "on_saveas_clicked()" << std::endl;
}

bool 
Refactor::on_quit_clicked(GdkEventButton *ev)
{
  std::cerr << "on_button_clicked()" << std::endl;

  hide(); //hide() will cause main::run() to end.

  return 0;
}

bool 
Refactor::on_copy_clicked(GdkEventButton *ev)
{
  std::cerr << "on_copy_clicked()" << std::endl;
}

bool 
Refactor::on_cut_clicked(GdkEventButton *ev)
{
  std::cerr << "on_cut_clicked()" << std::endl;
}

bool 
Refactor::on_paste_clicked(GdkEventButton *ev)
{
  std::cerr << "on_paste_clicked()" << std::endl;
}

bool 
Refactor::on_delete_clicked(GdkEventButton *ev)
{
  std::cerr << "on_delete_clicked()" << std::endl;
}

bool 
Refactor::on_about_clicked(GdkEventButton *ev)
{
  std::cerr << "on_about_clicked()" << std::endl;
}
