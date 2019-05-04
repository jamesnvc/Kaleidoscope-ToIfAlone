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

//Constructor with input key and layer number
ToIfAlone::KeyBinding::KeyBinding(Key input_, uint16_t layer_) {
  input = input_;
  layer = layer_;
}

ToIfAlone::KeyBinding * ToIfAlone::map;
Key ToIfAlone::current_pressed_;
bool ToIfAlone::using_layer_ = false;

ToIfAlone::ToIfAlone() {
  // Default to empty map
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

  if (current_pressed_.raw == Key_NoKey.raw &&
      (hid::wasModifierKeyActive(Key_LeftShift) ||
       hid::wasModifierKeyActive(Key_RightShift) ||
       hid::wasModifierKeyActive(Key_LeftGui) ||
       hid::wasModifierKeyActive(Key_RightGui) ||
       hid::wasModifierKeyActive(Key_RightControl) ||
       hid::wasModifierKeyActive(Key_LeftControl) ||
       hid::wasModifierKeyActive(Key_RightAlt) ||
       hid::wasModifierKeyActive(Key_LeftAlt))) {
    return mapped_key;
  }

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
      // A mapped key is being held and another key was pressed
      // Get the pressed key in the layer the current key shifts to
      using_layer_ = true;
      for (uint8_t i = 0; map[i].input.raw != Key_NoKey.raw; i++) {
        if (map[i].input.raw == current_pressed_.raw) {
          return Layer.getKey(map[i].layer, row, col);
        }
      }
    }
  } else if (keyIsPressed(key_state) && current_pressed_.raw != Key_NoKey.raw) {
    // If a mapped key is currently being pressed, pretend no key is
    // being pressed for now
    if (current_pressed_.raw == mapped_key.raw) {
      return Key_NoKey;
    }
    // If a mapped key is held and some other key is pressed, send the
    // pressed key in the shifted layer
    using_layer_ = true;
    for (uint8_t i = 0; map[i].input.raw != Key_NoKey.raw; i++) {
      if (map[i].input.raw == current_pressed_.raw) {
          return Layer.getKey(map[i].layer, row, col);
      }
    }
  } else if (keyToggledOff(key_state) &&
             current_pressed_.raw != Key_NoKey.raw &&
             current_pressed_.raw == mapped_key.raw) {
    // If a mapped key was held and has been released..
    if (!using_layer_) {
      // ...send the mapped key by itself if no other keys were pressed
      handleKeyswitchEvent(current_pressed_, row, col, IS_PRESSED | INJECTED);
      hid::sendKeyboardReport();
    } else {
      KeyboardHardware.maskHeldKeys();
    }
    // ...and in any case, clear the state
    current_pressed_.raw = Key_NoKey.raw;
    using_layer_ = false;
  }
  // Default to returning the inputted key
  return mapped_key;
}

}

kaleidoscope::ToIfAlone ToIfAlone;
