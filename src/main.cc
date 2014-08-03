// TODO: this includes entire gtkmm headers
// replace with only the ones needed
//#include <gtkmm.h>
#include <gtkmm/window.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/builder.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/textview.h>

#include <glibmm.h>

#include <iostream>

Gtk::Window* pWindow = 0;

Gtk::Statusbar* pStatusBar = 0;

Gtk::EventBox* pPullCurrentPatch = 0;
Gtk::EventBox* pPushCurrentPatch = 0;
Gtk::EventBox* pPullAllPatches = 0;
Gtk::EventBox* pPushAllPatches = 0;

Gtk::ImageMenuItem* pNew = 0;
Gtk::ImageMenuItem* pOpen = 0;
Gtk::ImageMenuItem* pSave = 0;
Gtk::ImageMenuItem* pSaveAs = 0;
Gtk::ImageMenuItem* pQuit = 0;

Gtk::ImageMenuItem* pCopy = 0;
Gtk::ImageMenuItem* pCut = 0;
Gtk::ImageMenuItem* pPaste = 0;
Gtk::ImageMenuItem* pDelete = 0;

Gtk::ImageMenuItem* pAbout = 0;

Glib::RefPtr<Gtk::TextBuffer> pBuffer;
Gtk::TextView* pOutput = 0;

extern "C" {
  int old_main(const char *, int , int);
}

static
bool 
on_pull_current(GdkEventButton *)
{
  int result = old_main("test.wav", 4, 16);

  return result;
}

static
bool 
on_pull_all(GdkEventButton *)
{
  int result = old_main("test.wav", 4, 16);

  return result;
}

static
bool 
on_push_current(GdkEventButton *)
{
  int result = old_main("test.wav", 4, 16);

  return result;
}

static
bool 
on_push_all(GdkEventButton *)
{
  int result = old_main("test.wav", 4, 16);

  return result;
}

//
// Signal Handlers for the Menu Items
//
static
void 
on_new_clicked()
{
  std::cerr << "on_new_clicked()" << std::endl;
}

static
void 
on_open_clicked()
{
  //std::cerr << "on_open_clicked()" << std::endl;
  pBuffer->insert(pBuffer->end(),"on_open_clicked()\n");
}

static
void 
on_save_clicked()
{
  std::cerr << "on_save_clicked()" << std::endl;
}

static
void 
on_saveas_clicked()
{
  std::cerr << "on_saveas_clicked()" << std::endl;
}

static
void 
on_quit_clicked()
{
  std::cerr << "on_button_clicked()" << std::endl;

  if (pWindow)
    pWindow->hide(); //hide() will cause main::run() to end.
}

static
void 
on_copy_clicked()
{
  std::cerr << "on_copy_clicked()" << std::endl;
}

static
void on_cut_clicked()
{
  std::cerr << "on_cut_clicked()" << std::endl;
}

static
void on_paste_clicked()
{
  std::cerr << "on_paste_clicked()" << std::endl;
}

static
void 
on_delete_clicked()
{
  std::cerr << "on_delete_clicked()" << std::endl;
}

static
void 
on_about_clicked()
{
  std::cerr << "on_about_clicked()" << std::endl;
}

//
//
//
Glib::RefPtr<Gtk::Builder> 
loadGlade()
{
  Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();
  try
  {
    refBuilder->add_from_file("basic.glade");
  }
  catch(const Glib::FileError& ex)
  {
    std::cerr << "FileError: " << ex.what() << std::endl;
    //return NULL;
  }
  catch(const Glib::MarkupError& ex)
  {
    std::cerr << "MarkupError: " << ex.what() << std::endl;
    //return NULL;
  }
  catch(const Gtk::BuilderError& ex)
  {
    std::cerr << "BuilderError: " << ex.what() << std::endl;
    //return NULL;
  }

  return refBuilder;
}

void 
loadStatusBar(Glib::RefPtr<Gtk::Builder> refBuilder)
{
  refBuilder->get_widget("StatusBar", pStatusBar);
  if (pStatusBar)
    {
      guint context = pStatusBar->get_context_id("reFactor");
      pStatusBar->push("Starting",context);
    }

}

void 
attachSignalHandlers(Glib::RefPtr<Gtk::Builder> refBuilder)
{
  // Retrieve pointers to push/pull icons
  refBuilder->get_widget("evbPullCurrentPatch", pPullCurrentPatch);
  if (pPullCurrentPatch)
    {
      pPullCurrentPatch->signal_button_press_event().connect( sigc::ptr_fun(&on_pull_current) );
    }

  refBuilder->get_widget("evbPushCurrentPatch", pPushCurrentPatch);

  if (pPushCurrentPatch)
    {
      pPushCurrentPatch->signal_button_press_event().connect( sigc::ptr_fun(&on_push_current) );
    }

  refBuilder->get_widget("evbPullAllPatches", pPullAllPatches);
  
  if (pPullAllPatches)
    {
      pPullAllPatches->signal_button_press_event().connect( sigc::ptr_fun(&on_pull_all) );
    }

  refBuilder->get_widget("evbPushAllPatches", pPushAllPatches);
  
  if (pPushAllPatches)
    {
      pPushAllPatches->signal_button_press_event().connect( sigc::ptr_fun(&on_push_all) );
    }

  //
  //Get the GtkBuilder-instantiated menu items, and connect a signal handlers:
  //

  // File menu options
  refBuilder->get_widget("mnuNew", pNew);

  if (pNew)
    {   
      pNew->signal_activate().connect( sigc::ptr_fun(&on_new_clicked) );
    }

  refBuilder->get_widget("mnuOpen", pOpen);

  if (pOpen)
    {   
      pOpen->signal_activate().connect( sigc::ptr_fun(&on_open_clicked) );
    }

  refBuilder->get_widget("mnuSave", pSave);

  if (pSave)
    {   
      pSave->signal_activate().connect( sigc::ptr_fun(&on_save_clicked) );
    }

  refBuilder->get_widget("mnuSaveAs", pSaveAs);

  if (pSaveAs)
    {   
      pSaveAs->signal_activate().connect( sigc::ptr_fun(&on_saveas_clicked) );
    }

  refBuilder->get_widget("mnuQuit", pQuit);

  if (pQuit)
    {   
      pQuit->signal_activate().connect( sigc::ptr_fun(&on_quit_clicked) );
    }

  // Edit menu options
  refBuilder->get_widget("mnuCopy", pCopy);

  if (pCopy)
    {   
      pCopy->signal_activate().connect( sigc::ptr_fun(&on_copy_clicked) );
    }

  refBuilder->get_widget("mnuCut", pCut);

  if (pCut)
    {   
      pCut->signal_activate().connect( sigc::ptr_fun(&on_cut_clicked) );
    }

  refBuilder->get_widget("mnuPaste", pPaste);

  if (pPaste)
    {   
      pPaste->signal_activate().connect( sigc::ptr_fun(&on_paste_clicked) );
    }

  refBuilder->get_widget("mnuDelete", pDelete);

  if (pDelete)
    {   
      pDelete->signal_activate().connect( sigc::ptr_fun(&on_delete_clicked) );
    }

  // Help menu items
  refBuilder->get_widget("mnuAbout", pAbout);

  if (pAbout)
    {   
      pAbout->signal_activate().connect( sigc::ptr_fun(&on_about_clicked) );
    }

}

void
initOutput(Glib::RefPtr<Gtk::Builder> refBuilder)
{
  refBuilder->get_widget("txtOutput", pOutput);
  pBuffer = pOutput->get_buffer();
  pBuffer->erase(pBuffer->begin(),pBuffer->end());
}

int 
main(int argc, char *argv[])
{
  
  /* 
     creates a Gtk::Application object, stored in a RefPtr smartpointer. This is needed
     in all gtkmm applications. The create() method for this object initializes gtkmm, 
     and checks the arguments passed to your application on the command line, looking 
     for standard options such as --display. It takes these from the argument list, 
     leaving anything it does not recognize for your application to parse or ignore. 
     This ensures that all gtkmm applications accept the same set of standard arguments.
   */
  Glib::RefPtr<Gtk::Application> app = 
    Gtk::Application::create(argc, argv,"org.gtkmm.examples.base");

  // Load UI from Glade file
  Glib::RefPtr<Gtk::Builder> refBuilder = loadGlade();

  // Get status bar
  loadStatusBar(refBuilder);


  // Attach the signal handlers
  attachSignalHandlers(refBuilder);

  /*
    Get the GtkBuilder-instantiated main Window:
   */

  refBuilder->get_widget("RefactorWindow", pWindow);

  initOutput(refBuilder);

  // Check that the window was successfully found before using
  if (pWindow)
  {
    //Set the default window size
    pWindow->resize(400,400);

    app->run(*pWindow);
  }

  delete pWindow;

  return 0;

  
}

