// zmq api is so simple enough
// we don't encapsulate it in bff_common

#pragma once

#define BFF_LOGGER_ADDRESS "tcp://127.0.0.1:5559"

#include "OS/Thread.hpp"

// system include
#include <zmq.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <sstream>