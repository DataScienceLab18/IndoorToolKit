#ifndef SRC_GLOBAL_H_
#define SRC_GLOBAL_H_

#pragma once

#include <boost/random.hpp>
#include <boost/random/random_device.hpp>

extern boost::random::random_device rd;
extern boost::random::mt19937 gen;

#endif  // SRC_GLOBAL_H_
