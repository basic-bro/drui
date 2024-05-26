// user/App.cpp

#include "core/ui/Widgets.hpp"

// BEGIN USER EDIT //
#include "user/ui/Widgets.hpp"
int const w = 300;
int const h = 600;
// END USER EDIT //


int AppWidth()
{
  return w;
}

int AppHeight()
{
  return h;
}

Key App()
{
  return AppRoot(
    AppWidth(), AppHeight(),
    ChatApp()   // USER: CHANGE THIS TO YOUR APP FUNCTION
  );
}