/* -*- mode: c++ -*-
 * Kaleidoscope-ToIfAlone -- Karabiner's ToIfAlone functionality for Kaleidoscope
 * Copyright (C) 2017  James Cash
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <Kaleidoscope.h>

#ifndef TOIFALONE_MAP_END
#define TOIFALONE_MAP_END (kaleidoscope::ToIfAlone::KeyBinding) { Key_NoKey, 0 }
#endif

namespace kaleidoscope {
class ToIfAlone : public KaleidoscopePlugin {
  public:
   class KeyBinding {
     public:
      Key input;
      uint16_t layer;
   };

   ToIfAlone(void);

   void begin(void) final;

   static ToIfAlone::KeyBinding * map;
  private:
   static Key current_pressed_;
   static bool using_layer_;
   static Key eventHandlerHook(Key mapped_key, byte row, byte col, uint8_t key_state);
};
};

extern kaleidoscope::ToIfAlone ToIfAlone;
