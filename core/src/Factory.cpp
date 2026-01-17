
#include "mixr/simulation/factory.hpp"
#include "mixr/base/IObject.hpp"

#include "core/CrfsSimulation.h"
#include "core/CPR_Generator.h"
#include "core/EmissionManager.h"
#include "core/EmitterSettingsManager.h"
#include "core/NetPacketGenerator.h"
#include "core/Propegator.h"
#include "core/ReflectionManager.h"
#include "core/RemoteHandler.h"
#include "core/WorldManager.h"

#include <string>

namespace mixr {
	namespace crfs {
        //todo namespace core and just factory
		base::IObject* crfsFactory(const std::string& name)
		{
			base::IObject* obj{};

            if (name == CrfsSimulation::getFactoryName()) { obj = new CrfsSimulation(); }
            if (name == CPR_Generator::getFactoryName()) { obj = new CPR_Generator(); }
			if (name == EmissionManager::getFactoryName() )   { obj = new EmissionManager(); }
			if (name == EmitterSettingsManager::getFactoryName()) { obj = new EmitterSettingsManager(); }
			if (name == NetPacketGenerator::getFactoryName()) { obj = new NetPacketGenerator(); }
			if (name == Propegator::getFactoryName()) { obj = new Propegator(); }
			if (name == ReflectionManager::getFactoryName()) { obj = new ReflectionManager();  }
            if (name == RemoteHandler::getFactoryName()) { obj = new RemoteHandler(); }
			if (name == WorldManager::getFactoryName()) { obj = new WorldManager(); }
			return obj;
		}
	}
}

