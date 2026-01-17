
#include "mixr/base/IObject.hpp"
#include "gui/SdlVisual.h"

#include <string>

namespace mixr {
	namespace crfs {
		base::IObject* crfsGuiFactory(const std::string& name)
		{
			base::IObject* obj{};

			if ( name == SdlVisual::getFactoryName() )   { obj = new SdlVisual(); }
			return obj;
		}
	}
}

