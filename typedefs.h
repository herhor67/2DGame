#pragma once

#include <type_traits>

typedef std::make_signed<size_t>::type BlkCrd; // Block Coordinate Type
typedef std::make_signed<size_t>::type ChkCrd; // Chunk Coordinate Type

typedef float EntCrd; // Entity Coordinate Type

typedef float TDT; // Time Difference Type

typedef unsigned char ClrT; // color type
