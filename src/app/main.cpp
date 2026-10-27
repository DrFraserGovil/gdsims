#include "settings/MainSettings.h"
#include <JSL.h>
int main(int argc, char **argv)
{
	ChangeSettings().Parse(argc, argv);

	LOG(INFO) << Settings.Runtime.Quiet;
}
