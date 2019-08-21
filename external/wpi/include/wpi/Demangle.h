/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_DEMANGLE_H_
#define WPIUTIL_WPI_DEMANGLE_H_

#include <string>

namespace wpi {

/**
 * Demangle a C++ symbol.
 *
 * @param mangledSymbol the mangled symbol.
 * @return The demangled symbol, or mangledSymbol if demangling fails.
 */
std::string Demangle(char const* mangledSymbol);

}  // namespace wpi

#endif  // WPIUTIL_WPI_DEMANGLE_H_
