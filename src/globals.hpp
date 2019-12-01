#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <bx/allocator.h>
#include <memory>

#include "resourceManager.hpp"

extern std::unique_ptr<bx::AllocatorI> allocator;
extern resourceManager resources;

#endif
