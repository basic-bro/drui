// main.cpp

#include "core/Application.hpp"
#include "core/App.hpp"

#include<iostream>

int main()
{
	try
	{
		Application app{ AppWidth(), AppHeight(), App() };
		app.Run(); 
		return 0;
	}
	catch( std::exception const& e )
	{
		std::cerr << "\nException caught: " << e.what() << '\n';
	}
}