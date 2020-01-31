/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "acc_filter.h"

static float filter_taps[ACCFILTER_TAP_NUM] = {
  0.08590479619727703,
  -0.09561795182582299,
  -0.06211030438770355,
  -0.32981053775368185,
  0.7991830011150345,
  -0.32981053775368185,
  -0.06211030438770355,
  -0.09561795182582299,
  0.08590479619727703
};

void accFilter_init(accFilter* f) {
  int i;
  for(i = 0; i < ACCFILTER_TAP_NUM; ++i)
    f->history[i] = 0;
  f->last_index = 0;
}

void accFilter_put(accFilter* f, float input) {
  f->history[f->last_index++] = input;
  if(f->last_index == ACCFILTER_TAP_NUM)
    f->last_index = 0;
}

float accFilter_get(accFilter* f) {
  float acc = 0;
  int index = f->last_index, i;
  for(i = 0; i < ACCFILTER_TAP_NUM; ++i) {
    index = index != 0 ? index-1 : ACCFILTER_TAP_NUM-1;
    acc += f->history[index] * filter_taps[i];
  };
  return acc;
}
