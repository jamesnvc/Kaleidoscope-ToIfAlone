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

#include <Kaleidoscope-ToIfAlone.h>
#include <kaleidoscope/hid.h>

namespace kaleidoscope {

//Constructor with input and output
ToIfAlone::KeyBinding::KeyBinding(Key input_, uint16_t layer_) {
  input = input_;
  layer = layer_;
}

ToIfAlone::KeyBinding * ToIfAlone::map;
Key ToIfAlone::current_pressed_;
bool ToIfAlone::using_layer_ = false;

ToIfAlone::ToIfAlone() {
  static ToIfAlone::KeyBinding defaultmap[] = {
    TOIFALONE_MAP_END
  };
  map = defaultmap;
  current_pressed_.raw = Key_NoKey.raw;
}

void ToIfAlone::begin() {
  Kaleidoscope.useEventHandlerHook(eventHandlerHook);
}

Key ToIfAlone::eventHandlerHook(Key mapped_key, byte row, byte col, uint8_t key_state) {
  if (key_state & INJECTED)
    return mapped_key;

  if (!keyIsPressed(key_state) && !keyWasPressed(key_state)) {
    if (current_pressed_.raw != Key_NoKey.raw) {
      return Key_NoKey;
    }
    for (uint8_t i = 0; map[i].input.raw != Key_NoKey.raw; i++) {
      if (map[i].input.raw == mapped_key.raw) {
        return Key_NoKey;
      }
    }
    return mapped_key;
  }

  if (keyToggledOn(key_state)) {
    // If we aren't already holding down a mapped key, check if the
    // just pressed key is one
    if (current_pressed_.raw == Key_NoKey.raw) {
      for (uint8_t i = 0; map[i].input.raw != Key_NoKey.raw; i++) {
        if (map[i].input.raw == mapped_key.raw) {
          // If we have a match, record we saw that and pretend no key
          // was pressed for now
          current_pressed_.raw = mapped_key.raw;
          return Key_NoKey;
        }
      }
    } else {
      using_layer_ = true;
      for (uint8_t i = 0; map[i].input.raw != Key_NoKey.raw; i++) {
        if (map[i].input.raw == current_pressed_.raw) {
          return Layer.getKey(map[i].layer, row, col);
        }
      }
    }
  } else if (keyIsPressed(key_state) && current_pressed_.raw != Key_NoKey.raw) {
    if (current_pressed_.raw == mapped_key.raw) {
      return Key_NoKey;
    }
    using_layer_ = true;
    for (uint8_t i = 0; map[i].input.raw != Key_NoKey.raw; i++) {
      if (map[i].input.raw == current_pressed_.raw) {
          return Layer.getKey(map[i].layer, row, col);
      }
    }
  } else if (keyToggledOff(key_state) &&
             current_pressed_.raw != Key_NoKey.raw &&
             current_pressed_.raw == mapped_key.raw) {
    if (!using_layer_) {
      handleKeyswitchEvent(current_pressed_, row, col, IS_PRESSED | INJECTED);
      hid::sendKeyboardReport();
    }
    current_pressed_.raw = Key_NoKey.raw;
    using_layer_ = false;
  }
  return mapped_key;
}

}

kaleidoscope::ToIfAlone ToIfAlone;
