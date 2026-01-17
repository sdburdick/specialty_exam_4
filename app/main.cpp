
#include <filesystem>

#include "mixr/simulation/Station.hpp"
#include "mixr/base/edl_parser.hpp"
#include "mixr/base/Pair.hpp"
#include "mixr/base/numeric/Integer.hpp"
#include "mixr/base/units/angles.hpp"
#include "mixr/base/util/system_utils.hpp"

#include "CRFS_Packet_for_Receivers.h"
#include "gui/SdlVisual.h"

#include "mixr/graphics/display.hpp"
// factories
#include "shared/xzmq/factory.hpp" //this one is here just to show how to get to the shared area
#include "mixr/simulation/factory.hpp"
#include "mixr/models/factory.hpp"
#include "mixr/models/dynamics/jsbsim/factory.hpp"
#include "mixr/interop/dis/factory.hpp"
#include "mixr/terrain/factory.hpp"
#include "mixr/base/factory.hpp"
#include "mixr/graphics/factory.hpp"

#include "core/Factory.h"
#include "remote/Factory.h"
#include "gui/Factory.h"

#include <string>
#include <cstdlib>

#define SDL_MAIN_HANDLED //wild - without this caused a random 'main' to get invoked up in mixr, makeTheFont.cpp which then ignored the main() here
#include <SDL2/SDL.h>
#include <chrono>
#include <thread>

//here I'm automating the preprocessor step.  default file name can be overridden by command line with a '-p'
std::string configFilename{ "settings/crfs.mix" };
//this function will be called if a '-f' is not passed in through the command line (i.e. you can override the auto-precompile by using -f "file.mixr")
int runMcpp(const std::string& input, const std::string& output) {

    std::string ext = "";
    #ifdef _WIN32
        ext = ".exe";
    #endif
    std::filesystem::path absoluteExe = std::filesystem::absolute("settings/mcpp" + ext); 
    
    if (!std::filesystem::exists(absoluteExe)) {
        std::cerr << "Error: Could not find mcpp at " << absoluteExe << std::endl;
        return -1;
    }
    std::string command = "\"" + absoluteExe.string() + "\" \"" + input + "\" > \"" + output + "\"";
    #ifdef _WIN32
        command = "\"" + command + "\"";
    #endif

    std::cout << "Executing: " << command << std::endl;
    return std::system(command.c_str());
}

//this block, including the GetCPUInfo func are tied under this ifdef.  this just prints out useful debug info and can be removed
#ifdef WIN32
	#include <iostream>
	#include <windows.h>
	#include <wbemidl.h>

	#pragma comment(lib, "wbemuuid.lib")

	void GetCPUInfo() {
		HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
		if (FAILED(hres)) return;

		hres = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
			RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
		if (FAILED(hres)) return;

		IWbemLocator* pLoc = NULL;
		hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator,
			(LPVOID*)&pLoc);
		if (FAILED(hres)) return;

		IWbemServices* pSvc = NULL;
		hres = pLoc->ConnectServer(BSTR(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSvc);
		if (FAILED(hres)) return;

		hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
			RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
		if (FAILED(hres)) return;

		IEnumWbemClassObject* pEnumerator = NULL;
		hres = pSvc->ExecQuery(BSTR(L"WQL"), BSTR(L"SELECT * FROM Win32_Processor"),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
		if (FAILED(hres)) return;

		IWbemClassObject* pclsObj = NULL;
		ULONG uReturn = 0;
		while (pEnumerator) {
			hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
			if (uReturn == 0) break;

			VARIANT vtProp;
			VariantInit(&vtProp);
			pclsObj->Get(L"L2CacheSize", 0, &vtProp, 0, 0);
			std::wcout << "L2 Cache Size: " << vtProp.uintVal << " KB\n";
			VariantClear(&vtProp);

			pclsObj->Get(L"L3CacheSize", 0, &vtProp, 0, 0);
			std::wcout << "L3 Cache Size: " << vtProp.uintVal << " KB\n";
			VariantClear(&vtProp);

			pclsObj->Get(L"MaxClockSpeed", 0, &vtProp, 0, 0);
			std::wcout << "Processor Speed: " << vtProp.uintVal << " MHz\n";
			VariantClear(&vtProp);

			pclsObj->Release();
		}

		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
	}
#endif

//const std::string& input 
using FactoryFunc = mixr::base::IObject* (*)(const std::string&);

FactoryFunc factories[] = {//array of IObject returning factory pointers
    mixr::crfs::crfsFactory,
    mixr::crfs::remoteFactory,
    mixr::crfs::crfsGuiFactory,
    mixr::simulation::factory,
    mixr::models::factory,
    mixr::terrain::factory,
    mixr::dis::factory,
    mixr::base::factory,
    mixr::graphics::factory
};

mixr::base::IObject* factory(const std::string& name)
{
    for (auto& f : factories) {
        if (auto obj = f(name)) return obj;
    }
    return nullptr;
}

// station builder
mixr::simulation::Station* builder(const std::string& filename)
{
   // read configuration file
   int num_errors{};
   mixr::base::IObject* obj{mixr::base::edl_parser(filename, factory, &num_errors)};
   if (num_errors > 0) {
      std::cerr << "File: " << filename << ", number of errors: " << num_errors << std::endl;
      std::exit(EXIT_FAILURE);
   }

   // test to see if an object was created
   if (obj == nullptr) {
      std::cerr << "Invalid configuration file, no objects defined!" << std::endl;
      std::exit(EXIT_FAILURE);
   }

   // do we have a base::Pair, if so, point to object in Pair, not Pair itself
   const auto pair = dynamic_cast<mixr::base::Pair*>(obj);
   if (pair != nullptr) {
      obj = pair->object();
      obj->ref();
      pair->unref();
   }

   // We are using a station as our top level object, and expect it here.  
   const auto station = dynamic_cast<mixr::simulation::Station*>(obj);
   if (station == nullptr) {
      std::cerr << "Invalid configuration file - top level object must be a Station or a Station within a Pair" << std::endl;
      std::exit(EXIT_FAILURE);
   }
   return station;
}

int main(int argc, char* argv[])
{
	
#ifdef WIN32
    //this just calls the debug function and can be deleted if necessary
	GetCPUInfo();
#endif

    std::stringstream outputFilename;
    outputFilename << configFilename;
    outputFilename << "r";
    bool autoPrecompile = true;

	for (int i{ 1 }; i < argc; i++) { 
		if (std::string(argv[i]) == "-f") {
            outputFilename.clear();
            outputFilename << argv[++i]; //already precompiled file entered
            autoPrecompile = false;
            break; //first time we find a -f we process, not taking any others
		}
        if (std::string(argv[i]) == "-p") {
            configFilename = argv[++i]; //file to be precompiled.  we add on an 'r' to the end to show complete (mix->mixr)
            outputFilename.clear();
            outputFilename << argv[++i]; 
            outputFilename << "r";
            break; //first time we find a -p we process, not taking any others
        }
	}

    if (autoPrecompile) {
        int ret = runMcpp(configFilename, outputFilename.str());
        if (ret != 0) {
            std::cerr << "Error: mcpp preprocessing failed!" << configFilename << " " << outputFilename.str() << std::endl;
            return ret;
        }
    }


	// Parse the input file (local to this main)
	mixr::simulation::Station* station{ builder(outputFilename.str()) };

	// send a reset event
	station->event(mixr::base::IComponent::RESET_EVENT);
    
    //this whole section should be moved into a file specifically for GUIs
    //todo

    auto mySdlVisual = station->findComponent<mixr::crfs::SdlVisual>(station);
    auto myInputDisplay = station->findComponent<mixr::graphics::Display>(station);
    
    // create time critical thread
	station->createTimeCriticalProcess();
	// short pause to allow os to startup thread
	mixr::base::msleep(20);
    //create a background loop within station
    station->createBackgroundProcess();
    //create the net thread
    station->createNetworkProcess();

	// system Time of Day
	double simTime{};                                        // Simulator time reference
	const double startTime{ mixr::base::getComputerTime() };   // Time of day (sec) run started

	std::cout << "Main.cpp : starting the GUI\n" << std::endl;

	
	//initialize all the GL stuff:
	   //we are using SDL2 as the interface to GL
	auto sdlInit = SDL_Init(SDL_INIT_VIDEO);
	if (sdlInit != 0) {
		std::cout << "SDL_Init error: " << SDL_GetError() << "\n";
		return 1;//close the program
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    //todo - why am i stuck using old gl?  Why would i need GLEW? or GLAD?  read up on the SDL2 instructions
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);


    SDL_Window* window = SDL_CreateWindow("OpenGL with SDL2",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, context);

    bool guiRunning = true;
    while (guiRunning) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                guiRunning = false;
                break;
            }
        }
        if (mySdlVisual != nullptr) {
            mySdlVisual->draw(window, context); // Handles glClear + drawing
        }

        SDL_GL_SwapWindow(window);          // Presents the frame
        //SDL_Delay(16); // ~60 FPS
        SDL_Delay(32); // 
    }

	station->event(mixr::base::IComponent::SHUTDOWN_EVENT);
	while (station->isNotShutdown()){
		mixr::base::msleep(10);
	}
	std::cout << "ready to close\n";
    station->unref();

    
   return 0;
}
