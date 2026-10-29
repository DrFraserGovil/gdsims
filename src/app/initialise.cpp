#include "initialise.h"
#include "settings/MainSettings.h"
#include "settings/derive.h"
#include "settings/validate.h"
#include <JSL/Log.h>
#include <JSL/Strings.h>

namespace
{
	bool singleCmd(const std::vector<std::string> &cmds, const std::string &target)
	{
		return cmds.size() == 1 && JSL::String::iEquals(cmds[0], target);
	}
	Mode parseCommands()
	{
		const auto &cmds = Settings.Commands;
		LOG(DEBUG) << "Parsing Commands " << cmds;

		if (cmds.empty() || singleCmd(cmds, "run"))
		{
			return Mode::Run;
		}

		if (singleCmd(cmds, "interactive"))
		{
			ChangeSettings().Runtime.Interactive = true;
			return Mode::Interactive;
		}
		if (singleCmd(cmds, "version"))
		{
			return Mode::Version;
		}

		LOG(ERROR) << "Unknown or conflicting commands passed to the application\n\t" << cmds;
		throw std::invalid_argument("Could not parse commands");
	}
} // namespace

Mode readConfiguration(int argc, char **argv)
{
	ChangeSettings().Parse(argc, argv);
	deriveSettingsValues();

	auto mode = parseCommands();

	switch (mode)
	{
		case Mode::Version: {
			break; // version doesn't validate
		}
		case Mode::Interactive: {

			// Interactive mode allows an invalid startup, as long as nothing is fatal: the menu must refuse to launch a simulation until the errors are fixed
			const auto status = querySettingsConstraints(Settings);
			status.LogWarnings();
			status.ThrowIfFatal();
			if (!status.Valid())
			{
				status.LogErrors();
				LOG(INFO) << "These must be fixed in interactive mode before a simulation can be launched";
			}
			break;
		}
		case Mode::Run: {
			validateSettings(Settings);
			break;
		}
	}

	return mode;
}
