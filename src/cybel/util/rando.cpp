/*
 * This file is part of EkoScape.
 * Copyright (c) 2024 Bradley Whited
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "rando.h"

namespace cybel {

Rando::Rando() {}

Rando& Rando::it() {
  static Rando it_{};
  return it_;
}

bool Rando::rand_bool() { return bool_distr_(gen_); }

double Rando::rand_double() { return double_distr_(gen_); }

double Rando::rand_double(double min,double max) {
  return double_distr_(gen_,std::uniform_real_distribution<double>::param_type(min,max));
}

float Rando::rand_float() { return float_distr_(gen_); }

float Rando::rand_float(float min,float max) {
  return float_distr_(gen_,std::uniform_real_distribution<float>::param_type(min,max));
}

int Rando::rand_int() { return int_distr_(gen_); }

int Rando::rand_int(int max) { return rand_int(0,max); }

int Rando::rand_int(int min,int max) {
  return int_distr_(gen_,std::uniform_int_distribution<int>::param_type(min,max - 1));
}

} // Namespace.
