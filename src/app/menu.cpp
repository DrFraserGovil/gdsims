#include "menu.h"
#include "commands.h"
#include "settings/derive.h"
#include "settings/validate.h"
#include <JSL.h>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

const auto cmdCol = JSL::Display::Cyan();
// Prompts and output the user asked for go to std::cout, never LOG: the log level (-q / -s) could hide them. Banners and hints may use LOG (hiding them under -q is fine)
namespace
{
	using Tokens = std::vector<std::string>;

	// Splits on any whitespace. TODO: quote-aware splitting, for arguments containing spaces
	Tokens tokenise(const std::string &line)
	{
		Tokens tokens;
		std::istringstream ss(line);
		std::string token;
		while (ss >> token)
		{
			tokens.push_back(token);
		}
		return tokens;
	}

	// Aggregator::Parse treats -h / -help / help as "print help, then exit(0)", which would kill the menu, so these are caught first
	bool requestsHelp(const Tokens &tokens)
	{
		return std::any_of(tokens.begin(), tokens.end(), [](const std::string &t) {
			const auto bare = std::string_view(t).substr(std::min(t.find_first_not_of('-'), t.size()));
			return JSL::String::iEquals(bare, "h") || JSL::String::iEquals(bare, "help");
		});
	}

	// Restores the global Settings (and anything derived from them) when it goes out of scope, so overrides from `run [args]` only last for that run, even if it throws
	class SettingsRestorer
	{
	  public:
		SettingsRestorer() : saved(Settings) {}
		~SettingsRestorer()
		{
			ChangeSettings() = saved;
			deriveSettingsValues();
		}
		SettingsRestorer(const SettingsRestorer &) = delete;
		SettingsRestorer &operator=(const SettingsRestorer &) = delete;

	  private:
		const MainSettings saved;
	};

	// Parses the menu line exactly as if it had been passed on the command line (so -config works too), layered on top of the current settings
	void applyOverrides(Tokens tokens)
	{
		tokens.insert(tokens.begin(), "gdsims"); // argv[0]
		std::vector<char *> argv;
		for (auto &t : tokens)
		{
			argv.push_back(t.data());
		}
		argv.push_back(nullptr); // argv is conventionally null-terminated

		ChangeSettings().Parse(static_cast<int>(tokens.size()), argv.data());
		deriveSettingsValues();
	}

	void runCommand(const Tokens &tokens)
	{
		if (requestsHelp(tokens))
		{
			ChangeSettings().Help(); // the command-line help is exactly the syntax that `run [args]` accepts
			return;
		}

		SettingsRestorer restore; // harmless when there are no overrides
		if (tokens.size() > 1)
		{
			applyOverrides(tokens);
			if (Settings.Commands.size() != 1)
			{
				throw std::invalid_argument("run only accepts -flag arguments, but was also given other commands");
			}
		}

		// Checked here (rather than letting runSimulation throw) so that a bad state is reported in the usual validation format, and the user stays in the menu
		const auto status = querySettingsConstraints(Settings);
		status.LogWarnings();
		status.ThrowIfFatal();
		if (!status.Valid())
		{
			// A reminder rather than a log entry: one message, in the error colour, without LOG's headers
			std::cerr << JSL::Log::Global().ErrorColour << "Simulation not launched; the settings still need fixing:\n\t"
					  << JSL::String::stitch(status.Errors, "\n\t") << JSL::Display::ResetAll() << "\n";
			return;
		}
		runSimulation();
	}

	void settingsMenu()
	{
		LOG(INFO) << JSL::Display::Yellow(true) << "\t-- Configuration Interface --\t\t" << JSL::Display::ResetAll();
		ChangeSettings() = JSL::Interface::Console(Settings, {.ExitString = "save", .ConsoleString = "gdsims-config > "});

		deriveSettingsValues();
		auto status = querySettingsConstraints(Settings);

		if (!status.Valid())
		{
			LOG(WARN) << "The settings are in an invalid state: a simulation will refuse to launch until this is rectified";
		}
		status.LogWarnings();
		status.LogErrors();
		status.ThrowIfFatal();

		// Reconfigure and reparse
	}

	// TODO: generate from a command table (shared with dispatch) once the command list settles
	void printMenuHelp()
	{
		// setw sits inside the colour codes, so the (invisible) escape sequences don't count towards the padding
		auto entry = [](std::string_view cmd, std::string_view description) {
			std::cout << "\t" << cmdCol << std::left << std::setw(12) << cmd << std::right << JSL::Display::DefaultColour() << description << "\n";
		};

		std::cout << "Available commands:\n";
		entry("run [args]", "Run a simulation; any -flag arguments apply to that run only");
		entry("settings", "Open the configuration interface");
		entry("dump", "(not yet implemented)");
		entry("version", "Print the version number");
		entry("help", "Show this list");
		entry("quit", "Exit GDSiMS");
	}

	// Returns false when the user asks to leave the menu
	// TODO: complain about unexpected arguments to settings/dump/version (currently ignored)
	bool dispatch(const Tokens &tokens)
	{
		auto is = [&](std::string_view name) { return JSL::String::iEquals(tokens[0], name); };

		if (is("quit") || is("exit"))
		{
			return false;
		}

		if (is("help"))
		{
			printMenuHelp();
		}
		else if (is("run"))
		{
			runCommand(tokens);
		}
		else if (is("settings"))
		{
			settingsMenu();
		}
		else if (is("dump"))
		{
			dump();
		}
		else if (is("version"))
		{
			printVersion();
		}
		else
		{
			std::cout << "Unrecognised command '" << tokens[0] << "' (type 'help' for a list)\n";
		}
		return true;
	}
} // namespace

void openMenu()
{
	LOG(INFO) << JSL::Display::Purple(true) << "\t-- Interactive GDSiMS --\t\t" << JSL::Display::ResetAll();

	// only print the help without asking in normal/verbose mode
	LOG(INFO) << "Type " << cmdCol << "help" << JSL::Display::DefaultColour() << " for command list";

	std::string line;
	while (true)
	{
		std::cout << "gdsims> " << cmdCol << std::flush;

		if (!std::getline(std::cin, line))
		{
			std::cout << JSL::Display::DefaultColour() << "\n"; // EOF (Ctrl-D): leave cleanly
			return;
		}
		std::cout << JSL::Display::DefaultColour() << std::flush;

		const auto tokens = tokenise(line);
		if (tokens.empty())
		{
			continue;
		}

		try
		{
			if (!dispatch(tokens))
			{
				return;
			}
		}
		catch (std::exception &e)
		{
			// A failed command (e.g. run with invalid settings) is reported, then we return to the prompt rather than exiting
			std::cerr << "ERROR: " << e.what() << "\n";
		}
	}
}
