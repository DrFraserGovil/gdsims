#include "initialise.h"
#include "settings/MainSettings.h"
#include "settings/validate.h"
#include <JSL.h>
int main(int argc, char **argv)
{
	try
	{
		auto mode = readConfiguration(argc, argv);
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
