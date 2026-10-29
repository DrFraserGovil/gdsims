#include "commands.h"
#include "initialise.h"
#include "menu.h"
#include "test.h"
#include <JSL.h>
int main(int argc, char **argv)
{
	Settings test;
	test.Parse(argc, argv);
	// try
	// {
	// 	const Mode mode = readConfiguration(argc, argv);
	// 	switch (mode)
	// 	{
	// 		case Mode::Run:
	// 			runSimulation();
	// 			break;
	// 		case Mode::Interactive:
	// 			openMenu();
	// 			break;
	// 		case Mode::Version:
	// 			printVersion();
	// 			break;
	// 	}
	// }
	// catch (std::exception &e)
	// {
	// 	std::cerr << "FATAL ERROR: " << e.what() << "\n";
	// 	LOG(INFO) << "GDSiMS cannot recover: exiting with code 1";
	// 	return 1;
	// }

	return 0;
}
