#include <iostream>
#include <njr/algebra/polynomials/polynomials.hpp>
#include <njr/algebra/rings/rings.hpp>

using namespace njr::algebra;

int main() {
    using E = Residue<11>::Element;
    using Poly = Polynomial<E>;
    const auto x = Poly::variable();
    const auto polynomial = x * x + Poly::one();
    const auto divisor = x + Poly::one();
    const auto division = polynomial.divmod(divisor);
    std::cout << polynomial << '\n';
    std::cout << division.first << '\n';
    std::cout << division.second << '\n';
}
