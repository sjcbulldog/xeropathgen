#include "QuadraticSolver.h"
#include <cmath>

namespace xero {
    namespace paths {

        std::vector<double> QuadraticSolver::solve(double a, double b, double c) {
            std::vector<double> result ;
            double tmp = b * b - 4 * a * c ;

            if (tmp == 0.0) {
                result.push_back(-b/(2 * a)) ;
            }
            else if (tmp > 0.0) {
                result.push_back((-b + std::sqrt(tmp)) / (2 * a)) ;
                result.push_back((-b - std::sqrt(tmp)) / (2 * a)) ;

                if (result[0] < result[1]) {
                    //
                    // Swap the result, the biggest should always be first
                    //
                    double tmp2 = result[0] ;
                    result[0] = result[1] ;
                    result[1] = tmp2 ;
                }
            }
            return result ;
        }
    }
}
