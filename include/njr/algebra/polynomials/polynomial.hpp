#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include <njr/algebra/core/core.hpp>

namespace njr::algebra {

template<RingElement T>
class PolynomialRing;

template<RingElement T>
class Polynomial : public AdditiveOperators<Polynomial<T>>,
                   public MultiplicativeOperators<Polynomial<T>> {
    std::vector<T> coefficients_;

    void normalize() {
        while (!coefficients_.empty() && coefficients_.back().is_zero()) {
            coefficients_.pop_back();
        }
    }

public:
    using coefficient_type = T;
    using parent_type = PolynomialRing<T>;
    using algebra = std::conditional_t<IntegralDomainElement<T>, IntegralDomainDescriptor,
        std::conditional_t<CommutativeRingElement<T>, CommutativeRingDescriptor, RingDescriptor>>;

    Polynomial() = default;

    Polynomial(std::initializer_list<T> coefficients) : coefficients_(coefficients) {
        normalize();
    }

    explicit Polynomial(std::vector<T> coefficients) : coefficients_(std::move(coefficients)) {
        normalize();
    }

    explicit Polynomial(const T& constant) : coefficients_{constant} {
        normalize();
    }

    static Polynomial zero() {
        return Polynomial{};
    }

    static Polynomial one() {
        return Polynomial{T::one()};
    }

    static Polynomial variable() {
        return Polynomial{T::zero(), T::one()};
    }

    static Polynomial from_integer(std::int64_t value)
        requires requires { T::from_integer(value); }
    {
        return Polynomial{T::from_integer(value)};
    }

    bool is_zero() const {
        return coefficients_.empty();
    }

    bool is_one() const {
        if (coefficients_.empty()) {
            return T::zero().is_one();
        }
        return coefficients_.size() == 1 && coefficients_[0].is_one();
    }

    std::optional<std::size_t> degree() const {
        if (is_zero()) {
            return std::nullopt;
        }
        return coefficients_.size() - 1;
    }

    T coefficient(std::size_t index) const {
        if (index >= coefficients_.size()) {
            return T::zero();
        }
        return coefficients_[index];
    }

    const std::vector<T>& coefficients() const {
        return coefficients_;
    }

    T leading_coefficient() const {
        if (is_zero()) {
            throw std::domain_error("The zero polynomial has no leading coefficient");
        }
        return coefficients_.back();
    }

    void set_coefficient(std::size_t index, const T& value) {
        if (index >= coefficients_.max_size()) {
            throw std::length_error("Polynomial extent exceeds the supported range");
        }
        if (index >= coefficients_.size()) {
            if (value.is_zero()) {
                return;
            }
            coefficients_.resize(index + 1, T::zero());
        }
        coefficients_[index] = value;
        normalize();
    }

    Polynomial add(const Polynomial& other) const {
        const auto count = std::max(coefficients_.size(), other.coefficients_.size());
        std::vector<T> result(count, T::zero());
        for (std::size_t i = 0; i < count; ++i) {
            result[i] = coefficient(i).add(other.coefficient(i));
        }
        return Polynomial(std::move(result));
    }

    Polynomial negate() const {
        std::vector<T> result;
        result.reserve(coefficients_.size());
        for (const auto& value : coefficients_) {
            result.push_back(value.negate());
        }
        return Polynomial(std::move(result));
    }

    Polynomial multiply(const Polynomial& other) const {
        if (is_zero() || other.is_zero()) {
            return zero();
        }
        const auto maximum = coefficients_.max_size();
        if (coefficients_.size() > maximum - other.coefficients_.size() + 1) {
            throw std::length_error("Polynomial product exceeds the supported range");
        }
        const auto count = coefficients_.size() + other.coefficients_.size() - 1;
        std::vector<T> result(count, T::zero());
        for (std::size_t i = 0; i < coefficients_.size(); ++i) {
            for (std::size_t j = 0; j < other.coefficients_.size(); ++j) {
                const T product = coefficients_[i].multiply(other.coefficients_[j]);
                result[i + j] = result[i + j].add(product);
            }
        }
        return Polynomial(std::move(result));
    }

    Polynomial scale_left(const T& factor) const {
        std::vector<T> result;
        result.reserve(coefficients_.size());
        for (const auto& value : coefficients_) {
            result.push_back(factor.multiply(value));
        }
        return Polynomial(std::move(result));
    }

    Polynomial scale_right(const T& factor) const {
        std::vector<T> result;
        result.reserve(coefficients_.size());
        for (const auto& value : coefficients_) {
            result.push_back(value.multiply(factor));
        }
        return Polynomial(std::move(result));
    }

    T evaluate(const T& argument) const {
        T result = T::zero();
        for (std::size_t i = coefficients_.size(); i > 0; --i) {
            const T product = result.multiply(argument);
            result = product.add(coefficients_[i - 1]);
        }
        return result;
    }

    std::pair<Polynomial, Polynomial> divmod(const Polynomial& divisor) const {
        if (divisor.is_zero()) {
            throw std::domain_error("Cannot divide by the zero polynomial");
        }
        T leading_inverse = T::one();
        if (!divisor.leading_coefficient().is_one()) {
            if constexpr (FieldElement<T>) {
                leading_inverse = divisor.leading_coefficient().reciprocal();
            } else {
                throw std::domain_error("A monic divisor is required for these coefficients");
            }
        }
        Polynomial quotient;
        Polynomial remainder = *this;
        const auto divisor_degree = *divisor.degree();
        while (!remainder.is_zero() && *remainder.degree() >= divisor_degree) {
            const auto shift = *remainder.degree() - divisor_degree;
            const T factor = remainder.leading_coefficient().multiply(leading_inverse);
            quotient.set_coefficient(shift, quotient.coefficient(shift).add(factor));
            for (std::size_t j = 0; j <= divisor_degree; ++j) {
                const T product = factor.multiply(divisor.coefficient(j));
                const T difference = remainder.coefficient(shift + j).add(product.negate());
                remainder.set_coefficient(shift + j, difference);
            }
        }
        return {std::move(quotient), std::move(remainder)};
    }

    Polynomial operator/(const Polynomial& divisor) const {
        return divmod(divisor).first;
    }

    Polynomial operator%(const Polynomial& divisor) const {
        return divmod(divisor).second;
    }

    const parent_type& parent() const {
        static const parent_type instance;
        return instance;
    }

    bool operator==(const Polynomial& other) const {
        return coefficients_ == other.coefficients_;
    }

    friend std::ostream& operator<<(std::ostream& out, const Polynomial& polynomial)
        requires requires(std::ostream& stream, const T& value) { stream << value; }
    {
        if (polynomial.is_zero()) {
            return out << T::zero();
        }
        bool first = true;
        for (std::size_t i = 0; i < polynomial.coefficients_.size(); ++i) {
            if (polynomial.coefficients_[i].is_zero()) {
                continue;
            }
            if (!first) {
                out << " + ";
            }
            out << polynomial.coefficients_[i];
            if (i > 0) {
                out << "*x^" << i;
            }
            first = false;
        }
        return out;
    }
};

template<RingElement T>
class PolynomialRing : public RingStructureFor<Polynomial<T>> {
public:
    using Element = Polynomial<T>;

    static Element element(std::initializer_list<T> coefficients) {
        return Element(coefficients);
    }
};

template<FieldElement T>
Polynomial<T> polynomial_gcd(Polynomial<T> a, Polynomial<T> b) {
    while (!b.is_zero()) {
        Polynomial<T> remainder = a % b;
        a = std::move(b);
        b = std::move(remainder);
    }
    if (a.is_zero()) {
        return a;
    }
    return a.scale_left(a.leading_coefficient().reciprocal());
}

}
