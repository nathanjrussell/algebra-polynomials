#include <cstdint>
#include <stdexcept>
#include <vector>
#include <gtest/gtest.h>
#include <algebra/polynomials/polynomials.hpp>
#include <algebra/rings/rings.hpp>

using namespace algebra;

struct TestField;

class TestCoefficient : public AdditiveOperators<TestCoefficient>,
                        public MultiplicativeOperators<TestCoefficient> {
    unsigned value_ = 0;

public:
    using parent_type = TestField;
    using algebra = FieldDescriptor;
    static TestCoefficient from_integer(std::int64_t n) {
        TestCoefficient result;
        result.value_ = static_cast<unsigned>((n % 5 + 5) % 5);
        return result;
    }
    static TestCoefficient zero() { return {}; }
    static TestCoefficient one() { return from_integer(1); }
    TestCoefficient add(const TestCoefficient& b) const { return from_integer(value_ + b.value_); }
    TestCoefficient multiply(const TestCoefficient& b) const { return from_integer(value_ * b.value_); }
    TestCoefficient negate() const { return from_integer(-static_cast<std::int64_t>(value_)); }
    TestCoefficient reciprocal() const {
        if (is_zero()) {
            throw std::domain_error("Zero has no reciprocal");
        }
        for (int n = 1; n < 5; ++n) {
            const auto candidate = from_integer(n);
            if (multiply(candidate).is_one()) {
                return candidate;
            }
        }
        throw std::logic_error("Inverse not found");
    }
    bool is_zero() const { return value_ == 0; }
    bool is_one() const { return value_ == 1; }
    bool operator==(const TestCoefficient& b) const { return value_ == b.value_; }
};

using E = Residue<11>::Element;
using Poly = Polynomial<E>;
using FieldPoly = Polynomial<TestCoefficient>;

template<class T>
concept CanFormPolynomial = requires { typename Polynomial<T>; };

static_assert(RingElement<Poly>);
static_assert(IntegralDomainElement<FieldPoly>);
static_assert(!FieldElement<FieldPoly>);
static_assert(!CanFormPolynomial<int>);

TEST(Polynomials, NormalizesZeroAndConstantIdentity) {
    EXPECT_EQ((Poly{E::one(), E::zero(), E::zero()}).degree(), 0u);
    EXPECT_TRUE((Poly{E::zero(), E::zero()}).is_zero());
    EXPECT_FALSE(Poly{}.degree().has_value());
    EXPECT_TRUE(Poly::one().is_one());
    EXPECT_TRUE((Poly::variable() + -Poly::variable()).is_zero());
}

TEST(Polynomials, HandlesTheZeroCoefficientRing) {
    using ZeroPoly = Polynomial<Residue<1>::Element>;
    EXPECT_TRUE(ZeroPoly::zero().is_one());
    EXPECT_TRUE(ZeroPoly::one().is_zero());
    EXPECT_TRUE(ZeroPoly::variable().is_zero());
}

TEST(Polynomials, MultipliesAndEvaluatesCoefficients) {
    const auto x = Poly::variable();
    const auto value = (x + Poly{E::one()}) * (x + Poly{E::from_integer(2)});
    EXPECT_EQ(value, (Poly{E::from_integer(2), E::from_integer(3), E::one()}));
    EXPECT_EQ(value.evaluate(E::from_integer(3)), E::from_integer(9));
    EXPECT_EQ(value.scale_left(E::from_integer(2)), value.scale_right(E::from_integer(2)));
}

TEST(Polynomials, DividesByMonicPolynomialsOverRings) {
    const Poly dividend{E::one(), E::from_integer(2), E::zero(), E::one()};
    const Poly divisor{E::one(), E::one()};
    const auto division = dividend.divmod(divisor);
    EXPECT_EQ(division.first, (Poly{E::from_integer(3), E::from_integer(10), E::one()}));
    EXPECT_EQ(division.second, (Poly{E::from_integer(9)}));
    EXPECT_EQ(division.first * divisor + division.second, dividend);
    EXPECT_LT(*division.second.degree(), *divisor.degree());
}

TEST(Polynomials, ChecksDivisionPreconditions) {
    const auto x = Poly::variable();
    EXPECT_THROW(x.divmod(Poly::zero()), std::domain_error);
    EXPECT_THROW(x.divmod(Poly{E::one(), E::from_integer(2)}), std::domain_error);
    EXPECT_THROW(Poly::zero().leading_coefficient(), std::domain_error);
}

TEST(Polynomials, DividesByNonmonicPolynomialsOverFields) {
    using F = TestCoefficient;
    const FieldPoly dividend{F::from_integer(2), F::zero(), F::one()};
    const FieldPoly divisor{F::one(), F::from_integer(2)};
    const auto division = dividend.divmod(divisor);
    EXPECT_EQ(division.first, (FieldPoly{F::one(), F::from_integer(3)}));
    EXPECT_TRUE(division.second.is_one());
    EXPECT_EQ(division.first * divisor + division.second, dividend);
}

TEST(Polynomials, ComputesMonicGreatestCommonDivisors) {
    using F = TestCoefficient;
    const auto x = FieldPoly::variable();
    const auto common = x - FieldPoly::one();
    const auto a = common * (x + FieldPoly{F::from_integer(2)});
    const auto b = common * (x + FieldPoly{F::from_integer(3)});
    EXPECT_EQ(polynomial_gcd(a, b), common);
    EXPECT_TRUE(polynomial_gcd(FieldPoly::zero(), FieldPoly::zero()).is_zero());
}

TEST(Polynomials, ExposesTheAppropriateVirtualParent) {
    const auto value = FieldPoly::variable();
    const interfaces::IntegralDomain<FieldPoly>& view = value.parent();
    EXPECT_TRUE(view.multiply(FieldPoly::one(), value) == value);
}
