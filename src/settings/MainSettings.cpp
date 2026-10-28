#include "MainSettings.h"

// This is a slightly winding path the ensure that Settings is a global object that can be accessed anywhere, but can only be changed by explicitly calling for it.

namespace // This ensures that the underlying object *only* exists within this translation unit
{
	MainSettings internalSettings;
} // namespace

// The 'settings' is actually the object named above; everyone else is only touching a reference to it
const MainSettings &Settings = internalSettings;

MainSettings &ChangeSettings()
{
	return internalSettings;
}
