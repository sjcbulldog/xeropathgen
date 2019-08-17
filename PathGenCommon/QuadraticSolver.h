#pragma once

#include <vector>

/// \file

namespace xero {
    namespace paths {

        /// \brief This class is not instantiated, but rather contains static methods to solve quadratics
        class QuadraticSolver {
        public:
            QuadraticSolver() = delete ;
            ~QuadraticSolver() = delete ;

            /// \brief Solve the quadratic equation given
            /// The parameters of the quadratic are given by a, b, and c.  All real
            /// roots of the quadratic are returns.  Imaginary roots are not.  The
            /// quadratic is represented in the form:<br> 
            // f(x) = a * x * x + b * x + c
            /// \param a parameter of the quadratic
            /// \param b parameter of the quadratic
            /// \param c parameter of the quadratic
            /// \returns the roots of the quadratic
            static std::vector<double> solve(double a, double b, double c) ;
        } ;
    }
}
