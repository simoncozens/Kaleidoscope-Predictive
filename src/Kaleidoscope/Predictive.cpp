/* -*- mode: c++ -*-
 * Kaleidoscope-Predictive -- Predictive LED effect for Kaleidoscope.
 * Copyright (C) 2018 Simon Cozens
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

#include <Kaleidoscope.h>
#include <Kaleidoscope-Predictive.h>

namespace kaleidoscope {

uint8_t Predictive::lru_[(ROWS+1)*COLS*LRU_SIZE];
uint8_t Predictive::lastKey_;
uint8_t* Predictive::last_lru_;
uint16_t Predictive::update_delay = 500;
uint32_t Predictive::end_time_;

void Predictive::debugLru() {
  if (!last_lru_) return;
  /*
  Serial.print("Dumping LRU at address ");
  Serial.print((int)last_lru_);
  Serial.print(" (last key was ");
  Serial.print(lastKey_);
  Serial.print(") : ");
  for (int i = 0; i < LRU_SIZE; i++) {
    Serial.print(last_lru_[i]);
    Serial.print(", ");
  }
  Serial.println("");
  */
}

/* We don't have the memory to do a proper bigram frequency map,
   so we basically fake it with a least-recently used list.
   Keycodes "bubble up" the list by swapping with their neighbours. */
void Predictive::updateLru(byte n_row, byte n_col) {
  if (!last_lru_) return;
  uint8_t keycode = RC2KEYCODE(n_row, n_col);
  /* Hunt through the list for the keycode */
  int pos = LRU_SIZE;
  for (int i = 0; i < LRU_SIZE; i++) {
    if (last_lru_[i] == keycode) {
      pos = i; break;
    }
    /* If there's an empty space, we'll go here */
    if(last_lru_[i] == 0) { pos = i; break;}
  }
  if (pos == 0) { /* We're at the top already (or the list is empty) */
    last_lru_[pos] = keycode;
    return;
  }
  uint8_t swap = last_lru_[pos-1];
  last_lru_[pos-1] = keycode;
  if (pos < LRU_SIZE) { last_lru_[pos] = swap; }

  debugLru();
}

void Predictive::predict() {
  if (!lastKey_) return;
  if (end_time_ && (millis() < end_time_))
    return;

  end_time_ = millis() + update_delay;

  byte r, c;
  ::LEDControl.set_all_leds_to({0, 0, 0});

  r = KEYCODE2ROW(last_lru_[0]);
  c = KEYCODE2COL(last_lru_[0]);
  if (!(r && c)) return;
  ::LEDControl.setCrgbAt(r, c, {255,200,200});
  r = KEYCODE2ROW(last_lru_[1]);
  c = KEYCODE2COL(last_lru_[1]);
  if (!(r && c)) return;
  ::LEDControl.setCrgbAt(r, c, {200,128,128});
  r = KEYCODE2ROW(last_lru_[2]);
  c = KEYCODE2COL(last_lru_[2]);
  if (!(r && c)) return;
  ::LEDControl.setCrgbAt(r, c, {128,64,64});

}


Predictive::Predictive(void) {
}

void Predictive::setup(void) {
  Kaleidoscope.useEventHandlerHook(eventHook);
}

Key Predictive::eventHook(Key mapped_key, byte row, byte col, uint8_t key_state) {
  // if it is a synthetic key, skip it.
  if (key_state & INJECTED)
    return mapped_key;

  // if the key is not toggled on, return.
  if (!keyToggledOn(key_state))
    return mapped_key;

  updateLru(row, col);
  lastKey_ = RC2KEYCODE(row,col);
  last_lru_ = (lru_ + lastKey_ * LRU_SIZE);
  return mapped_key;
}

void
Predictive::update(void) {
  predict();
}
}

kaleidoscope::Predictive PredictiveEffect;
