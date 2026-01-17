#include "gui/SdlVisual.h"

#include "mixr/base/util/system_utils.hpp"
#include "mixr/base/IComponent.hpp"
#include "mixr/base/Pair.hpp"
#include <SDL2/SDL.h>

#if defined(_WIN32)
#include <windows.h>
#include <GL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "remote/RemotePlayer.h" //attach the display to a player, then capture data to display
#include "remote/CPR_Receiver.h"

namespace mixr{
    namespace crfs {

        IMPLEMENT_SUBCLASS(SdlVisual, "SdlVisual")
            EMPTY_SLOTTABLE(SdlVisual)

        SdlVisual::SdlVisual() {
            STANDARD_CONSTRUCTOR()
                //todo - shared pointer
                own = 0;
            //auto conta = container();
              //  own = container()->findComponent<mixr::crfs::RemotePlayer>();
        }

        void SdlVisual::copyData(const SdlVisual& org, const bool cc) {
            BaseClass::copyData(org, cc);
        }

        void SdlVisual::deleteData() {
            BaseClass::deleteData();
        }

        void SdlVisual::updateData(const double dt) {
            // Update internal state if needed
            BaseClass::updateData(dt);
            own = container()->findComponent<mixr::crfs::RemotePlayer>();
        }

        void SdlVisual::draw(SDL_Window* window, SDL_GLContext context) {
            SDL_GL_MakeCurrent(window, context);

            if (!initialized) {
                initialized = true;

                // Set up viewport and orthographic projection
                glViewport(0, 0, 800, 600);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(0, 800, 600, 0, -1, 1); // left, right, bottom, top, near, far
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();

                

                glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // dark gray background
            }

            // Clear screen
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Set line width and color
            glLineWidth(5.0f); // thick line
            glColor3f(1.0f, 0.0f, 0.0f); // red

            if (own) {
                auto rx = own->findComponent<mixr::crfs::CPR_Receiver>();
                if (rx) {
                    auto count = rx->getMessageCount();
                    glBegin(GL_LINES);
                    glVertex2f(100.0f, count); // start point
                    glVertex2f(700.0f, -1.0f*count); // end point
                    glEnd();
                }
            }

            static float delta = 0;
            // Draw a horizontal line across the screen
            glBegin(GL_LINES);
            delta += 1.0;
            glVertex2f(100.0f+delta, 300.0f+delta); // start point
            if (delta > 250)
            glVertex2f(700.0f-delta+250.0, 300.0f-delta+250.0); // end point
            else
            glVertex2f(700.0f, 300.0f); // end point
            glEnd();
        }
    }
}