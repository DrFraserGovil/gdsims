#include "commands.h"
#include "initialise.h"
#include "menu.h"
#include <JSL.h>
int main(int argc, char **argv)
{
	try
	{
		const Mode mode = readConfiguration(argc, argv);
		switch (mode)
		{
			case Mode::Run:
				runSimulation();
				break;
			case Mode::Interactive:
				openMenu();
				break;
			case Mode::Version:
				printVersion();
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
