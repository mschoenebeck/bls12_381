#pragma once
#include <cmath>
#include <vector>

namespace bls12_381
{

class fp;
class fp2;
class fp6;
class fp12;

// g1 is type for point in G1.
// g1 is both used for Affine and Jacobian point representation.
// If z is equal to one the point is considered as in affine form.
class g1
{

public:
    fp x;
    fp y;
    fp z;

    g1();
    g1(const array<fp, 3>& e3);
    g1(const g1& e);
    static g1 fromJacobianBytesBE(const span<const uint8_t, 144> in, const bool check = false);
    static g1 fromAffineBytesBE(const span<const uint8_t, 96> in, const bool check = false);
    static g1 fromCompressedBytesBE(const span<const uint8_t, 48> in);
    void toJacobianBytesBE(const span<uint8_t, 144> out) const;
    void toAffineBytesBE(const span<uint8_t, 96> out) const;
    void toCompressedBytesBE(const span<uint8_t, 48> out) const;
    array<uint8_t, 144> toJacobianBytesBE() const;
    array<uint8_t, 96> toAffineBytesBE() const;
    array<uint8_t, 48> toCompressedBytesBE() const;
    static g1 zero();
    static g1 one();
    bool isZero() const;
    bool equal(const g1& e) const;
    bool inCorrectSubgroup() const;
    bool isOnCurve() const;
    bool isAffine() const;
    g1 affine() const;
    g1 add(const g1& e) const;
    g1 dbl() const;
    g1 neg() const;
    g1 sub(const g1& e) const;
    template<size_t N> g1 mulScalar(const array<uint64_t, N>& s) const;
    g1 clearCofactor() const;
    static g1 multiExp(const vector<g1>& points, vector<array<uint64_t, 4>>& powers);
    static g1 mapToCurve(const array<uint8_t, 48>& in);
    static tuple<fp, fp> swuMapG1(const fp& e);
    static void isogenyMapG1(fp& x, fp& y);

    static const g1 BASE;
    static const array<uint64_t, 1> cofactorEFF;
};

// g2 is type for point in G2.
// g2 is both used for Affine and Jacobian point representation.
// If z is equal to one the point is considered as in affine form.
class g2
{

public:
    fp2 x;
    fp2 y;
    fp2 z;

    g2();
    g2(const array<fp2, 3>& e3);
    g2(const g2& e);
    static g2 fromJacobianBytesBE(const span<const uint8_t, 288> in, const bool check = false);
    static g2 fromAffineBytesBE(const span<const uint8_t, 192> in, const bool check = false);
    static g2 fromCompressedBytesBE(const span<const uint8_t, 96> in);
    void toJacobianBytesBE(const span<uint8_t, 288> out) const;
    void toAffineBytesBE(const span<uint8_t, 192> out) const;
    void toCompressedBytesBE(const span<uint8_t, 96> out) const;
    array<uint8_t, 288> toJacobianBytesBE() const;
    array<uint8_t, 192> toAffineBytesBE() const;
    array<uint8_t, 96> toCompressedBytesBE() const;
    static g2 zero();
    static g2 one();
    bool isZero() const;
    bool equal(const g2& e) const;
    bool inCorrectSubgroup() const;
    bool isOnCurve() const;
    bool isAffine() const;
    g2 affine() const;
    g2 add(const g2& e) const;
    g2 dbl() const;
    g2 neg() const;
    g2 sub(const g2& e) const;
    template<size_t N> g2 mulScalar(const array<uint64_t, N>& s) const;
    g2 clearCofactor() const;
    g2 frobeniusMap(int64_t power) const;
    static g2 multiExp(const vector<g2>& points, vector<array<uint64_t, 4>>& powers);
    static g2 mapToCurve(const fp2& e);
    static g2 fromMessage(const vector<uint8_t>& msg, const string& dst);
    static tuple<fp2, fp2> swuMapG2(const fp2& e);
    //static void isogenyMapG2(fp2& x, fp2& y);
    g2 isogenyMap() const;

    static const g2 BASE;
    static const array<uint64_t, 1> cofactorEFF;
};

} // namespace bls12_381