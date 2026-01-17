
#include "mixr/simulation/factory.hpp"
#include "mixr/base/IObject.hpp"

#include "remote/CPR_Receiver.h"
#include "remote/EmitterSettings.h"
#include "remote/RemotePlayer.h"




#include <string>

namespace mixr {
	namespace crfs {
        //todo namespace core and just factory
		base::IObject* remoteFactory(const std::string& name)
		{
			base::IObject* obj{};

            if (name == CPR_Receiver::getFactoryName()) { obj = new CPR_Receiver(); }
            if (name == EmitterSettings::getFactoryName()) { obj = new EmitterSettings(); }
            if (name == RemotePlayer::getFactoryName()) { obj = new RemotePlayer(); }
			return obj;
		}
	}
}

