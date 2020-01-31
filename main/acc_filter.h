// Copyright (c) 2020 Marius Vikhammer
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef ACCFILTER_H_
#define ACCFILTER_H_

/*
sampling frequency: 50 Hz

* 0 Hz - 10 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = -42.41539049531685 dB

* 15 Hz - 25 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = 3.136281818184795 dB

*/

#define ACCFILTER_TAP_NUM 9

typedef struct {
  float history[ACCFILTER_TAP_NUM];
  unsigned int last_index;
} accFilter;

void accFilter_init(accFilter* f);
void accFilter_put(accFilter* f, float input);
float accFilter_get(accFilter* f);

#endif
