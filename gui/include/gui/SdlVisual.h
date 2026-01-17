#pragma once

#include <mixr/graphics/Graphic.hpp>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "remote/RemotePlayer.h"

using mixr::crfs::RemotePlayer;

namespace mixr {
    namespace crfs {

        class SdlVisual final : public mixr::graphics::Graphic {
            DECLARE_SUBCLASS(SdlVisual, mixr::graphics::Graphic)

        public:
            SdlVisual();

            void draw(SDL_Window* window, SDL_GLContext context);
            void updateData(const double dt) override;

        private:
            bool initialized{};
            RemotePlayer* own;
        };
    }
}
