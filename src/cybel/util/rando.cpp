/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "rando.h"

namespace cybel {

Rando& Rando::it() {
  static Rando it_{};

  return it_;
}

bool Rando::rand_bool() { return bool_distr_(gen_); }

double Rando::rand_double() { return double_distr_(gen_); }

double Rando::rand_double(double max) { return rand_double(0.0,max); }

double Rando::rand_double(double min,double max) {
  return double_distr_(gen_,std::uniform_real_distribution<>::param_type(min,max));
}

double Rando::rand_double_vel() { return rand_double() * rand_double_sign(); }

double Rando::rand_double_vel(double max) { return rand_double_vel(0.0,max); }

double Rando::rand_double_vel(double min,double max) { return rand_double(min,max) * rand_double_sign(); }

double Rando::rand_double_sign() { return rand_bool() ? 1.0 : -1.0; }

float Rando::rand_float() { return float_distr_(gen_); }

float Rando::rand_float(float max) { return rand_float(0.0f,max); }

float Rando::rand_float(float min,float max) {
  return float_distr_(gen_,std::uniform_real_distribution<float>::param_type(min,max));
}

float Rando::rand_float_vel() { return rand_float() * rand_float_sign(); }

float Rando::rand_float_vel(float max) { return rand_float_vel(0.0f,max); }

float Rando::rand_float_vel(float min,float max) { return rand_float(min,max) * rand_float_sign(); }

float Rando::rand_float_sign() { return rand_bool() ? 1.0f : -1.0f; }

int Rando::rand_int() { return int_distr_(gen_); }

int Rando::rand_int(int max) { return rand_int(0,max); }

int Rando::rand_int(int min,int max) {
  return int_distr_(gen_,std::uniform_int_distribution<>::param_type(min,max - 1));
}

int Rando::rand_int_vel() { return rand_int() * rand_int_sign(); }

int Rando::rand_int_vel(int max) { return rand_int_vel(0,max); }

int Rando::rand_int_vel(int min,int max) { return rand_int(min,max) * rand_int_sign(); }

int Rando::rand_int_sign() { return rand_bool() ? 1 : -1; }

std::size_t Rando::rand_sizet(std::size_t max) { return rand_sizet(0,max); }

std::size_t Rando::rand_sizet(std::size_t min,std::size_t max) {
  if(max == 0) { return 0; }

  return sizet_distr_(gen_,std::uniform_int_distribution<std::size_t>::param_type(min,max - 1));
}

} // namespace cybel
