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

#pragma once

#include <Kaleidoscope.h>
#include <Kaleidoscope-LEDControl.h>

#define RC2KEYCODE(r,c) ((r*(COLS+1))+c)
#define KEYCODE2COL(k) (k % (1+COLS))
#define KEYCODE2ROW(k) (k / (1+COLS))
#define LRU_SIZE 5

namespace kaleidoscope {
class Predictive : public LEDMode {
 public:
  Predictive(void);
  static uint16_t update_delay;

 protected:
  void setup(void) final;
  void update(void) final;

 private:
  static uint8_t lru_[(1+ROWS)*COLS * LRU_SIZE];
  static uint8_t* last_lru_;
  static uint8_t lastKey_;
  static uint32_t end_time_;
  static void updateLru(byte n_row, byte n_col);
  static void predict();
  static void debugLru();
  static Key eventHook(Key mapped_key, byte row, byte col, uint8_t key_state);
};
}

extern kaleidoscope::Predictive PredictiveEffect;
