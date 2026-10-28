#include "initialise.h"
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
				// TODO: launch the interactive menu
				break;
		}
	}
	catch (std::exception &e)
	{
		LOG(ERROR) << "FATAL ERROR: \n"
				   << e.what();
		LOG(INFO) << "GDSiMS cannot recover: exiting with code 1";
		return 1;
	}

	return 0;
}
