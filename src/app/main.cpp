#include "initialise.h"
#include "menu.h"
#include "version.h"
#include <JSL.h>
int main(int argc, char **argv)
{
	try
	{
		const Mode mode = readConfiguration(argc, argv);
		switch (mode)
		{
			case Mode::Run:
				// TODO: launch the simulation
				break;
			case Mode::Interactive:
				openMenu();
				break;
			case Mode::Version:
				std::cout << version::String << "\n";
				break;
			default:
				throw std::invalid_argument("Sorry, that command is not yet supoprted");
				break;
		}
	}
	catch (std::exception &e)
	{
		std::cerr << "FATAL ERROR: " << e.what() << "\n";
		LOG(INFO) << "GDSiMS cannot recover: exiting with code 1";
		return 1;
	}

	return 0;
}
