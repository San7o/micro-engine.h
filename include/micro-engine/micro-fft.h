// SPDX-License-Identifier: MIT
//
// fft.h
// =====
//
// Implementation of DFT and FFT in C99.
//
// Author:  Giovanni Santini
// Mail:    giovanni.santini@proton.me
// License: MIT
//

#ifndef MICRO_FFT_H
#define MICRO_FFT_H

#ifdef __cplusplus
extern "C" {
#endif

//
// Api
//

// Calculates the Discrete Furier Transform of [in_frames], saved in
// [out_frequencies]
void micro_dft(const float* in_frames,
               float *out_frequencies,
               unsigned int n_frames);

// The Fast Furier Transform of [in_frames], saved in [out_frequencies]
void micro_fft(const float* in_frames,
               float *out_frequencies,
               unsigned int window);

//
// Implementation
//
  
#ifdef MICRO_FFT_IMPLEMENTATION

#include <complex.h>

#ifndef PI
#define PI      3.14159265358979323846264f
#endif

void micro_dft(const float* in_frames, float *out_frequencies,
               unsigned int n_frames)
{
  // Calculate all frequency values in [out_frequencies] The number of
  // frequencies the the same as the number of discrete points
  // (frames) since we don't have enough information to calculate
  // more.
  for (unsigned int freq = 0; freq < n_frames; ++freq)
  {
    // A single frequency value represents how much that frequency
    // contributes to the overall mixed frequency (the input). To
    // calculate this, we multiply this frequency by the input for
    // each point, then we sum them up. This is done with different
    // offsets (sine and cosine) otherwise some information may be
    // never picked up (for example, if the input is a sine, and our
    // selected frequency is a cosine, the sum of their prducts will
    // always be zero and we don't get much information).
    out_frequencies[freq] = 0;
    for (unsigned int frame = 0; frame < n_frames; ++frame)
    {
      float t = (float) frame / n_frames;
      // Remember Euler's Formula
      //     e^{ix} = cos(x) + i * sin(x)
      // Using imaginary number is just a shortcut to encode information
      // for both sine and cosine.
      out_frequencies[freq] += in_frames[frame] * cexpf(-2 * I * PI * freq * t);
    }
  }
  return;
}

void micro_fft(const float* in_frames, float *out_frequencies,
               unsigned int window)
{
  // Base case
  if (window <= 1)
    return;

  // Split
  float even[window / 2], odd[window / 2];
  for (unsigned int i = 0; i < window / 2; i++)
  {
    even[i] = in_frames[i * 2];
    odd[i] = in_frames[i * 2 + 1];
  }
  micro_fft(even, out_frequencies, window / 2);
  micro_fft(odd, out_frequencies,  window / 2);

  // Combine
  for (unsigned int i = 0; i < window / 2; ++i)
  {
    complex float twiddle = cexp(-2.0 * I * PI * (float)i / (float)window) * odd[i];
    out_frequencies[i] = even[i] + twiddle;
    out_frequencies[i + window / 2] = even[i] - twiddle;
  }
  return;
}

#endif // MICRO_FFT_IMPLEMENTATION

#ifdef __cplusplus
}
#endif
  
#endif // MICRO_FFT_H

//
// FFT ts a very useful algorithm used in many domains.
//
//
// Sound
// -----
//
// Sound is essentially vibration of your ear, coming from a sound
// source and propagating through space. If the vibration follows some
// kind of pattern or frequency, then our head can make more sense of
// that, so overtime it tuned itself to prefere these patterns. There
// are some properties we give to sound frequency such as its volume
// (the amplitude), tune (frequency) and timbre (shape).
//
// There is a relationship between the lowest frequency that composes
// a periodic signal (aka the "Fundamental frequency") and the concept
// of an armonic series, which is a series of frequencies that we
// percieve as "good sounding".
//
// Suppose you are playing a frequency `f` (like the piano's C), then
// all the frequencies multiples of `f` (`2f`, `3f`, `4f`) would sound
// nice together, this happens because their sum produces a nice
// periodic signal.  If you map these frequency to the piano you get
// the famous piano chords. To do the mapping, you need to remember
// that each note in the piano multiples the previous one by 2^(1/21).
//
// We can find this frequency with the FFT by looking at the distance
// between the peaks of the most used frequencies.
//
// You can also use the FFT to do cool music visualizations, for
// example by visualizing the frequencies themselves, or using this
// data as parameters to drive some other kind of visualization
// (modifying shaped, colors...).
//
// One of the most direct ways to use the FFT is for creating an audio
// equalizer, which simply changes the frequencies in the frequency
// domain.
//

