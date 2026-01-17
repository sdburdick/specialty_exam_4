#pragma once

#include <string>

namespace mixr {
	namespace base { class IObject; }
	namespace crfs {
		base::IObject* crfsFactory(const std::string&);
	}
}

