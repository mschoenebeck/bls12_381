#include <array>
#include <vector>
#include <random>
#include <iostream>

#include "bls12_381.hpp"

using namespace std;

array<uint64_t, 4> random_scalar()
{
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    return {
        dis(gen) % fp::Q[0],
        dis(gen) % fp::Q[1],
        dis(gen) % fp::Q[2],
        dis(gen) % fp::Q[3]
    };
}

fp random_fe()
{
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    return fp({
        dis(gen) % 0xb9feffffffffaaab,
        dis(gen) % 0x1eabfffeb153ffff,
        dis(gen) % 0x6730d2a0f6b0f624,
        dis(gen) % 0x64774b84f38512bf,
        dis(gen) % 0x4b1ba7b6434bacd7,
        dis(gen) % 0x1a0111ea397fe69a
    });
}

fp2 random_fe2()
{
    return fp2({
        random_fe(),
        random_fe()
    });
}

fp6 random_fe6()
{
    return fp6({
        random_fe2(),
        random_fe2(),
        random_fe2()
    });
}

fp12 random_fe12()
{
    return fp12({
        random_fe6(),
        random_fe6()
    });
}

g1 random_g1()
{
    array<uint64_t, 4> k = random_scalar();
    return g1::one().mulScalar(k);
}

g2 random_g2()
{
    array<uint64_t, 4> k = random_scalar();
    return g2::one().mulScalar(k);
}

void TestFieldElementValidation()
{
    fp zero = fp::zero();
    if(!zero.isValid())
    {
        throw invalid_argument("zero must be valid");
    }
    fp one = fp::one();
    if(!one.isValid())
    {
        throw invalid_argument("one must be valid");
    }
    if(fp(fp::MODULUS).isValid())
    {
        throw invalid_argument("modulus must be invalid");
    }
    fp n = fp::MODULUS;
    n.d[0] += 1;
    if(n.isValid())
    {
        throw invalid_argument("number greater than modulus must be invalid");
    }
}

void TestFieldElementEquality()
{
    // fe
    fp zero = fp::zero();
    if(!zero.equal(zero))
    {
        throw invalid_argument("0 == 0");
    }
    fp one = fp::one();
    if(!one.equal(one))
    {
        throw invalid_argument("1 == 1");
    }
    fp a = random_fe();
    if(!a.equal(a))
    {
        throw invalid_argument("a == a");
    }
    fp b;
    _add(&b, &a, &one);
    if(a.equal(b))
    {
        throw invalid_argument("a != a + 1");
    }
    // fp2
    fp2 zero2 = fp2::zero();
    if(!zero2.equal(zero2))
    {
        throw invalid_argument("0 == 0");
    }
    fp2 one2 = fp2::one();
    if(!one2.equal(one2))
    {
        throw invalid_argument("1 == 1");
    }
    fp2 a2 = random_fe2();
    if(!a2.equal(a2))
    {
        throw invalid_argument("a == a");
    }
    fp2 b2;
    b2 = a2.add(one2);
    if(a2.equal(b2))
    {
        throw invalid_argument("a != a + 1");
    }
    // fp6
    fp6 zero6 = fp6::zero();
    if(!zero6.equal(zero6))
    {
        throw invalid_argument("0 == 0");
    }
    fp6 one6 = fp6::one();
    if(!one6.equal(one6))
    {
        throw invalid_argument("1 == 1");
    }
    fp6 a6 = random_fe6();
    if(!a6.equal(a6))
    {
        throw invalid_argument("a == a");
    }
    fp6 b6;
    b6 = a6.add(one6);
    if(a6.equal(b6))
    {
        throw invalid_argument("a != a + 1");
    }
    // fp12
    fp12 zero12 = fp12::zero();
    if(!zero12.equal(zero12))
    {
        throw invalid_argument("0 == 0");
    }
    fp12 one12 = fp12::one();
    if(!one12.equal(one12))
    {
        throw invalid_argument("1 == 1");
    }
    fp12 a12 = random_fe12();
    if(!a12.equal(a12))
    {
        throw invalid_argument("a == a");
    }
    fp12 b12;
    b12 = a12.add(one12);
    if(a12.equal(b12))
    {
        throw invalid_argument("a != a + 1");
    }
}

void TestFieldElementHelpers()
{
    // fe
    fp zero = fp::zero();
    if(!zero.isZero())
    {
        throw invalid_argument("'zero' is not zero");
    }
    fp one = fp::one();
    if(!one.isOne())
    {
        throw invalid_argument("'one' is not one");
    }
    fp odd = fp({1, 0, 0, 0, 0, 0});
    if(!odd.isOdd())
    {
        throw invalid_argument("1 must be odd");
    }
    if(odd.isEven())
    {
        throw invalid_argument("1 must not be even");
    }
    fp even = fp({2, 0, 0, 0, 0, 0});
    if(!even.isEven())
    {
        throw invalid_argument("2 must be even");
    }
    if(even.isOdd())
    {
        throw invalid_argument("2 must not be odd");
    }
    // fp2
    fp2 zero2 = fp2::zero();
    if(!zero2.isZero())
    {
        throw invalid_argument("'zero' is not zero, 2");
    }
    fp2 one2 = fp2::one();
    if(!one2.isOne())
    {
        throw invalid_argument("'one' is not one, 2");
    }
    // fp6
    fp6 zero6 = fp6::zero();
    if(!zero6.isZero())
    {
        throw invalid_argument("'zero' is not zero, 6");
    }
    fp6 one6 = fp6::one();
    if(!one6.isOne())
    {
        throw invalid_argument("'one' is not one, 6");
    }
    // fp12
    fp12 zero12 = fp12::zero();
    if(!zero12.isZero())
    {
        throw invalid_argument("'zero' is not zero, 12");
    }
    fp12 one12 = fp12::one();
    if(!one12.isOne())
    {
        throw invalid_argument("'one' is not one, 12");
    }
}

const size_t fuz = 10;

void TestFieldElementSerialization()
{
    {
        array<uint8_t, 48> in({0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0});
        fp e = fp::fromBytes(in);
        if(!e.isZero())
        {
            throw invalid_argument("bad serialization");
        }
        if(in != e.toBytes())
        {
            throw invalid_argument("bad serialization");
        }
    }
    {
        for(size_t i = 0; i < fuz; i++)
        {
            fp a = random_fe();
            fp b = fp::fromBytes(a.toBytes());
            if(!a.equal(b))
            {
                throw invalid_argument("bad serialization");
            }
        }
    }
    {
        for(size_t i = 0; i < fuz; i++)
        {
            fp a = random_fe();
            fp b = fp::fromString(a.toString());
            if(!a.equal(b))
            {
                throw invalid_argument("bad encoding or decoding");
            }
        }
    }
}

void TestFieldElementByteInputs()
{
    fp zero = fp::zero();
    array<uint8_t, 48> in = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    fp a = fp::fromBytes(in);
    if(!a.equal(zero))
    {
        throw invalid_argument("bad serialization");
    }
}

///////////////////////////////////////////////////////////

void TestG1Serialization()
{
    for(uint64_t i = 0; i < fuz; i++)
    {
        g1 a = random_g1();
        array<uint8_t, 96> buf = a.toBytes();
        g1 b = g1::fromBytes(buf);
        if(!a.equal(b))
        {
            throw invalid_argument("g1: bad serialization from/to");
        }
    }
}

void TestG1IsOnCurve()
{
    g1 zero = g1::zero();
    if(!zero.isOnCurve())
    {
        throw invalid_argument("zero must be on curve");
    }
    fp one = fp::one();
    g1 p = g1({one, one, one});
    if(p.isOnCurve())
    {
        throw invalid_argument("(1, 1) is not on curve");
    }
}

void TestG1AdditiveProperties()
{
    g1 t0, t1;
    g1 zero = g1::zero();
    for(uint64_t i = 0; i < fuz; i++)
    {
        g1 a = random_g1();
        g1 b = random_g1();
        t0 = a.add(zero);
        if(!t0.equal(a))
        {
            throw invalid_argument("a + 0 == a");
        }
        t0 = zero.add(zero);
        if(!t0.equal(zero))
        {
            throw invalid_argument("0 + 0 == 0");
        }
        t0 = a.sub(zero);
        if(!t0.equal(a))
        {
            throw invalid_argument("a - 0 == a");
        }
        t0 = zero.sub(zero);
        if(!t0.equal(zero))
        {
            throw invalid_argument("0 - 0 == 0");
        }
        t0 = zero.neg();
        if(!t0.equal(zero))
        {
            throw invalid_argument("- 0 == 0");
        }
        t0 = zero.sub(a);
        t0 = t0.neg();
        if(!t0.equal(a))
        {
            throw invalid_argument(" - (0 - a) == a");
        }
        t0 = zero.dbl();
        if(!t0.equal(zero))
        {
            throw invalid_argument("2 * 0 == 0");
        }
        t0 = a.dbl();
        t0 = t0.sub(a);
        if(!t0.equal(a) || !t0.isOnCurve())
        {
            throw invalid_argument(" (2 * a) - a == a");
        }
        t0 = a.add(b);
        t1 = b.add(a);
        if(!t0.equal(t1))
        {
            throw invalid_argument("a + b == b + a");
        }
        t0 = a.sub(b);
        t1 = b.sub(a);
        t1 = t1.neg();
        if(!t0.equal(t1))
        {
            throw invalid_argument("a - b == - ( b - a )");
        }
        g1 c = random_g1();
        t0 = a.add(b);
        t0 = t0.add(c);
        t1 = a.add(c);
        t1 = t1.add(b);
        if(!t0.equal(t1))
        {
            throw invalid_argument("(a + b) + c == (a + c ) + b");
        }
        t0 = a.sub(b);
        t0 = t0.sub(c);
        t1 = a.sub(c);
        t1 = t1.sub(b);
        if(!t0.equal(t1))
        {
            throw invalid_argument("(a - b) - c == (a - c) -b");
        }
    }
}

void TestG1MultiplicativePropertiesExpected()
{
    struct testdata {
        g1 g;
        array<uint64_t, 4> s1;
        array<uint64_t, 4> s2;
    };
    array<testdata, 1> tv = {
        testdata {
            g1({
                fp({3417891934972824866UL, 15033698923095482284UL, 4308006749155089476UL, 12106291570202121725UL, 2599277836029828640UL, 312782107632090658UL}),
                fp({5205587738984789115UL, 5067874624690748468UL, 13190631992216725609UL, 11345201024832610071UL, 14051629697167006557UL, 393909533259831005UL}),
                fp({3721446142051153446UL, 12426141085104671205UL, 9131026636729359854UL, 3777282351912617648UL, 11898443449281733652UL, 518350083385915957UL})
            }),
            {709522381180683898UL, 6180605729922878966UL, 17818642988822031089UL, 2436448532036587648UL},
            {13638846313248189432UL, 13997190380171701873UL, 13650465545368734818UL, 6581793109286319586UL}
        }
    };

    g1 t0, t1;
    g1 zero = g1::zero();
    for(uint64_t i = 0; i < 1; i++)
    {
        g1 a = tv[i].g;
        array<uint64_t, 4> s1 = tv[i].s1;
        array<uint64_t, 4> s2 = tv[i].s2;
        array<uint64_t, 10> s3;
        array<uint64_t, 4> sone = {1, 0, 0, 0};
        t0 = zero.mulScalar(s1);
        if(!t0.equal(zero))
        {
            throw invalid_argument(" 0 ^ s == 0");
        }
        t0 = a.mulScalar(sone);
        if(!t0.equal(a))
        {
            throw invalid_argument(" a ^ 1 == a");
        }
        t0 = zero.mulScalar(s1);
        if(!t0.equal(zero))
        {
            throw invalid_argument(" 0 ^ s == a");
        }
        t0 = a.mulScalar(s1);
        t0 = t0.mulScalar(s2);
        s3 = mul<10, 4, 4>(s1, s2);
        t1 = a.mulScalar(s3);
        if(!t0.equal(t1))
        {
            throw invalid_argument("G1: (a ^ s1) ^ s2 == a ^ (s1 * s2)");
        }
        t0 = a.mulScalar(s1);
        t1 = a.mulScalar(s2);
        t0 = t0.add(t1);
        s3 = add<10, 4, 4>(s1, s2);
        t1 = a.mulScalar(s3);
        if(!t0.equal(t1))
        {
            throw invalid_argument(" (a ^ s1) + (a ^ s2) == a ^ (s1 + s2)");
        }
    }
}

void TestG1MultiplicativeProperties()
{
    g1 t0, t1;
    g1 zero = g1::zero();
    for(uint64_t i = 0; i < fuz; i++)
    {
        g1 a = random_g1();
        array<uint64_t, 4> s1 = random_scalar();
        array<uint64_t, 4> s2 = random_scalar(); 
        array<uint64_t, 10> s3;
        array<uint64_t, 4> sone = {1, 0, 0, 0};
        t0 = zero.mulScalar(s1);
        if(!t0.equal(zero))
        {
            throw invalid_argument(" 0 ^ s == 0");
        }
        t0 = a.mulScalar(sone);
        if(!t0.equal(a))
        {
            throw invalid_argument(" a ^ 1 == a");
        }
        t0 = zero.mulScalar(s1);
        if(!t0.equal(zero))
        {
            throw invalid_argument(" 0 ^ s == a");
        }
        t0 = a.mulScalar(s1);
        t0 = t0.mulScalar(s2);
        s3 = mul<10, 4, 4>(s1, s2);
        t1 = a.mulScalar(s3);
        if(!t0.equal(t1))
        {
            throw invalid_argument("G1: (a ^ s1) ^ s2 == a ^ (s1 * s2)");
        }
        t0 = a.mulScalar(s1);
        t1 = a.mulScalar(s2);
        t0 = t0.add(t1);
        s3 = add<10, 4, 4>(s1, s2);
        t1 = a.mulScalar(s3);
        if(!t0.equal(t1))
        {
            throw invalid_argument(" (a ^ s1) + (a ^ s2) == a ^ (s1 + s2)");
        }
    }
}

void TestG1MultiExpExpected()
{
    g1 one = g1::one();
    vector<array<uint64_t, 4>> scalars = {
        {2, 0, 0, 0},
        {3, 0, 0, 0}
    };
    vector<g1> bases = {one, one};
    g1 expected, result;
    expected = one.mulScalar<1>({5});
    result = g1::multiExp(bases, scalars);
    if(!expected.equal(result))
    {
        throw invalid_argument("bad multi-exponentiation");
    }
}

void TestG1MultiExpBatch()
{
    g1 one = g1::one();
    int64_t n = 1000;
    vector<array<uint64_t, 4>> scalars;
    vector<g1> bases;
    // scalars: [s0,s1 ... s(n-1)]
    // bases: [P0,P1,..P(n-1)] = [s(n-1)*G, s(n-2)*G ... s0*G]
    for(int64_t i = 0, j = n-1; i < n; i++, j--)
    {
        scalars.insert(scalars.begin(), array<uint64_t, 4>{static_cast<uint64_t>(rand()%100000), 0, 0, 0});
        bases.push_back(g1::zero());
        bases[i] = one.mulScalar(scalars[0]);
    }
    // expected: s(n-1)*P0 + s(n-2)*P1 + s0*P(n-1)
    g1 expected, tmp;
    for(int64_t i = 0; i < n; i++)
    {
        tmp = bases[i].mulScalar(scalars[i]);
        expected = expected.add(tmp);
    }
    g1 result = g1::multiExp(bases, scalars);
    if(!expected.equal(result))
    {
        throw invalid_argument("bad multi-exponentiation");
    }
}

void TestG1MapToCurve()
{
    struct pair
    {
        array<uint8_t, 48> u;
        g1 expected;
    };
    array<pair, 5> pairs = {
        pair{
            array<uint8_t, 48>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            g1({fp::fromString("11a9a0372b8f332d5c30de9ad14e50372a73fa4c45d5f2fa5097f2d6fb93bcac592f2e1711ac43db0519870c7d0ea415"), fp::fromString("092c0f994164a0719f51c24ba3788de240ff926b55f58c445116e8bc6a47cd63392fd4e8e22bdf9feaa96ee773222133"), fp::one()})
        },
        pair{
            array<uint8_t, 48>{0x07, 0xfd, 0xf4, 0x9e, 0xa5, 0x8e, 0x96, 0x01, 0x5d, 0x61, 0xf6, 0xb5, 0xc9, 0xd1, 0xc8, 0xf2, 0x77, 0x14, 0x6a, 0x53, 0x3a, 0xe7, 0xfb, 0xca, 0x2a, 0x8e, 0xf4, 0xc4, 0x10, 0x55, 0xcd, 0x96, 0x1f, 0xbc, 0x6e, 0x26, 0x97, 0x9b, 0x55, 0x54, 0xe4, 0xb4, 0xf2, 0x23, 0x30, 0xc0, 0xe1, 0x6d},
            g1({fp::fromString("1223effdbb2d38152495a864d78eee14cb0992d89a241707abb03819a91a6d2fd65854ab9a69e9aacb0cbebfd490732c"), fp::fromString("0f925d61e0b235ecd945cbf0309291878df0d06e5d80d6b84aa4ff3e00633b26f9a7cb3523ef737d90e6d71e8b98b2d5"), fp::one()})
        },
        pair{
            array<uint8_t, 48>{0x12, 0x75, 0xab, 0x3a, 0xdb, 0xf8, 0x24, 0xa1, 0x69, 0xed, 0x4b, 0x1f, 0xd6, 0x69, 0xb4, 0x9c, 0xf4, 0x06, 0xd8, 0x22, 0xf7, 0xfe, 0x90, 0xd6, 0xb2, 0xf8, 0xc6, 0x01, 0xb5, 0x34, 0x84, 0x36, 0xf8, 0x97, 0x61, 0xbb, 0x1a, 0xd8, 0x9a, 0x6f, 0xb1, 0x13, 0x7c, 0xd9, 0x18, 0x10, 0xe5, 0xd2},
            g1({fp::fromString("179d3fd0b4fb1da43aad06cea1fb3f828806ddb1b1fa9424b1e3944dfdbab6e763c42636404017da03099af0dcca0fd6"), fp::fromString("0d037cb1c6d495c0f5f22b061d23f1be3d7fe64d3c6820cfcd99b6b36fa69f7b4c1f4addba2ae7aa46fb25901ab483e4"), fp::one()})
        },
        pair{
            array<uint8_t, 48>{0x0e, 0x93, 0xd1, 0x1d, 0x30, 0xde, 0x6d, 0x84, 0xb8, 0x57, 0x88, 0x27, 0x85, 0x6f, 0x5c, 0x05, 0xfe, 0xef, 0x36, 0x08, 0x3e, 0xef, 0x0b, 0x7b, 0x26, 0x3e, 0x35, 0xec, 0xb9, 0xb5, 0x6e, 0x86, 0x29, 0x96, 0x14, 0xa0, 0x42, 0xe5, 0x7d, 0x46, 0x7f, 0xa2, 0x09, 0x48, 0xe8, 0x56, 0x49, 0x09},
            g1({fp::fromString("15aa66c77eded1209db694e8b1ba49daf8b686733afaa7b68c683d0b01788dfb0617a2e2d04c0856db4981921d3004af"), fp::fromString("0952bb2f61739dd1d201dd0a79d74cda3285403d47655ee886afe860593a8a4e51c5b77a22d2133e3a4280eaaaa8b788"), fp::one()})
        },
        pair{
            array<uint8_t, 48>{0x01, 0x5a, 0x41, 0x48, 0x11, 0x55, 0xd1, 0x70, 0x74, 0xd2, 0x0b, 0xe6, 0xd8, 0xec, 0x4d, 0x46, 0x63, 0x2a, 0x51, 0x52, 0x1c, 0xd9, 0xc9, 0x16, 0xe2, 0x65, 0xbd, 0x9b, 0x47, 0x34, 0x3b, 0x36, 0x89, 0x97, 0x9b, 0x50, 0x70, 0x8c, 0x85, 0x46, 0xcb, 0xc2, 0x91, 0x6b, 0x86, 0xcb, 0x1a, 0x3a},
            g1({fp::fromString("06328ce5106e837935e8da84bd9af473422e62492930aa5f460369baad9545defa468d9399854c23a75495d2a80487ee"), fp::fromString("094bfdfe3e552447433b5a00967498a3f1314b86ce7a7164c8a8f4131f99333b30a574607e301d5f774172c627fd0bca"), fp::one()})
        },
    };
    for(uint64_t i = 0; i < 5; i++)
    {
        g1 p0 = g1::mapToCurve(pairs[i].u);
        if(!p0.equal(pairs[i].expected))
        {
            throw invalid_argument("map to curve fails");
        }
    }
}

///////////////////////////////////////////////////////////

void TestG2Serialization()
{
    for(uint64_t i = 0; i < fuz; i++)
    {
        g2 a = random_g2();
        array<uint8_t, 192> buf = a.toBytes();
        g2 b = g2::fromBytes(buf);
        if(!a.equal(b))
        {
            throw invalid_argument("g1: bad serialization from/to");
        }
    }
}

void TestG2IsOnCurve()
{
    g2 zero = g2::zero();
    if(!zero.isOnCurve())
    {
        throw invalid_argument("zero must be on curve");
    }
    fp2 one = fp2::one();
    g2 p = g2({one, one, one});
    if(p.isOnCurve())
    {
        throw invalid_argument("(1, 1) is not on curve");
    }
}

void TestG2AdditiveProperties()
{
    g2 t0, t1;
    g2 zero = g2::zero();
    for(uint64_t i = 0; i < fuz; i++)
    {
        g2 a = random_g2();
        g2 b = random_g2();
        t0 = a.add(zero);
        if(!t0.equal(a))
        {
            throw invalid_argument("a + 0 == a");
        }
        t0 = zero.add(zero);
        if(!t0.equal(zero))
        {
            throw invalid_argument("0 + 0 == 0");
        }
        t0 = a.sub(zero);
        if(!t0.equal(a))
        {
            throw invalid_argument("a - 0 == a");
        }
        t0 = zero.sub(zero);
        if(!t0.equal(zero))
        {
            throw invalid_argument("0 - 0 == 0");
        }
        t0 = zero.neg();
        if(!t0.equal(zero))
        {
            throw invalid_argument("- 0 == 0");
        }
        t0 = zero.sub(a);
        t0 = t0.neg();
        if(!t0.equal(a))
        {
            throw invalid_argument(" - (0 - a) == a");
        }
        t0 = zero.dbl();
        if(!t0.equal(zero))
        {
            throw invalid_argument("2 * 0 == 0");
        }
        t0 = a.dbl();
        t0 = t0.sub(a);
        if(!t0.equal(a) || !t0.isOnCurve())
        {
            throw invalid_argument(" (2 * a) - a == a");
        }
        t0 = a.add(b);
        t1 = b.add(a);
        if(!t0.equal(t1))
        {
            throw invalid_argument("a + b == b + a");
        }
        t0 = a.sub(b);
        t1 = b.sub(a);
        t1 = t1.neg();
        if(!t0.equal(t1))
        {
            throw invalid_argument("a - b == - ( b - a )");
        }
        g2 c = random_g2();
        t0 = a.add(b);
        t0 = t0.add(c);
        t1 = a.add(c);
        t1 = t1.add(b);
        if(!t0.equal(t1))
        {
            throw invalid_argument("(a + b) + c == (a + c ) + b");
        }
        t0 = a.sub(b);
        t0 = t0.sub(c);
        t1 = a.sub(c);
        t1 = t1.sub(b);
        if(!t0.equal(t1))
        {
            throw invalid_argument("(a - b) - c == (a - c) -b");
        }
    }
}

void TestG2MultiplicativeProperties()
{
    g2 t0, t1;
    g2 zero = g2::zero();
    for(uint64_t i = 0; i < fuz; i++)
    {
        g2 a = random_g2();
        array<uint64_t, 4> s1 = random_scalar();
        array<uint64_t, 4> s2 = random_scalar(); 
        array<uint64_t, 10> s3;
        array<uint64_t, 4> sone = {1, 0, 0, 0};
        t0 = zero.mulScalar(s1);
        if(!t0.equal(zero))
        {
            throw invalid_argument(" 0 ^ s == 0");
        }
        t0 = a.mulScalar(sone);
        if(!t0.equal(a))
        {
            throw invalid_argument(" a ^ 1 == a");
        }
        t0 = zero.mulScalar(s1);
        if(!t0.equal(zero))
        {
            throw invalid_argument(" 0 ^ s == a");
        }
        t0 = a.mulScalar(s1);
        t0 = t0.mulScalar(s2);
        s3 = mul<10, 4, 4>(s1, s2);
        t1 = a.mulScalar(s3);
        if(!t0.equal(t1))
        {
            throw invalid_argument("G2: (a ^ s1) ^ s2 == a ^ (s1 * s2)");
        }
        t0 = a.mulScalar(s1);
        t1 = a.mulScalar(s2);
        t0 = t0.add(t1);
        s3 = add<10, 4, 4>(s1, s2);
        t1 = a.mulScalar(s3);
        if(!t0.equal(t1))
        {
            throw invalid_argument(" (a ^ s1) + (a ^ s2) == a ^ (s1 + s2)");
        }
    }
}

void TestG2MultiExpExpected()
{
    g2 one = g2::one();
    vector<array<uint64_t, 4>> scalars = {
        {2, 0, 0, 0},
        {3, 0, 0, 0}
    };
    vector<g2> bases = {one, one};
    g2 expected, result;
    expected = one.mulScalar<1>({5});
    result = g2::multiExp(bases, scalars);
    if(!expected.equal(result))
    {
        throw invalid_argument("bad multi-exponentiation");
    }
}

void TestG2MultiExpBatch()
{
    g2 one = g2::one();
    int64_t n = 1000;
    vector<array<uint64_t, 4>> scalars;
    vector<g2> bases;
    // scalars: [s0,s1 ... s(n-1)]
    // bases: [P0,P1,..P(n-1)] = [s(n-1)*G, s(n-2)*G ... s0*G]
    for(int64_t i = 0, j = n-1; i < n; i++, j--)
    {
        scalars.insert(scalars.begin(), array<uint64_t, 4>{static_cast<uint64_t>(rand()%100000), 0, 0, 0});
        bases.push_back(g2::zero());
        bases[i] = one.mulScalar(scalars[0]);
    }
    // expected: s(n-1)*P0 + s(n-2)*P1 + s0*P(n-1)
    g2 expected, tmp;
    for(int64_t i = 0; i < n; i++)
    {
        tmp = bases[i].mulScalar(scalars[i]);
        expected = expected.add(tmp);
    }
    g2 result = g2::multiExp(bases, scalars);
    if(!expected.equal(result))
    {
        throw invalid_argument("bad multi-exponentiation");
    }
}

void TestG2MapToCurve()
{
    struct pair
    {
        array<uint8_t, 96> u;
        g2 expected;
    };
    array<pair, 5> pairs = {
        pair{
            array<uint8_t, 96>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            g2({fp2::fromString("0a67d12118b5a35bb02d2e86b3ebfa7e23410db93de39fb06d7025fa95e96ffa428a7a27c3ae4dd4b40bd251ac658892018320896ec9eef9d5e619848dc29ce266f413d02dd31d9b9d44ec0c79cd61f18b075ddba6d7bd20b7ff27a4b324bfce"), fp2::fromString("04c69777a43f0bda07679d5805e63f18cf4e0e7c6112ac7f70266d199b4f76ae27c6269a3ceebdae30806e9a76aadf5c0260e03644d1a2c321256b3246bad2b895cad13890cbe6f85df55106a0d334604fb143c7a042d878006271865bc35941"), fp2::one()})
        },
        pair{
            array<uint8_t, 96>{0x02, 0x5f, 0xbc, 0x07, 0x71, 0x1b, 0xa2, 0x67, 0xb7, 0xe7, 0x0c, 0x82, 0xca, 0xa7, 0x0a, 0x16, 0xfb, 0xb1, 0xd4, 0x70, 0xae, 0x24, 0xce, 0xef, 0x30, 0x7f, 0x5e, 0x20, 0x00, 0x75, 0x16, 0x77, 0x82, 0x0b, 0x70, 0x13, 0xad, 0x4e, 0x25, 0x49, 0x2d, 0xcf, 0x30, 0x05, 0x2d, 0x3e, 0x5e, 0xca, 0x0e, 0x77, 0x5d, 0x78, 0x27, 0xad, 0xf3, 0x85, 0xb8, 0x3e, 0x20, 0xe4, 0x44, 0x5b, 0xd3, 0xfa, 0xb2, 0x1d, 0x7b, 0x44, 0x98, 0x42, 0x6d, 0xaf, 0x3c, 0x1d, 0x60, 0x8b, 0x9d, 0x41, 0xe9, 0xed, 0xb5, 0xed, 0xa0, 0xdf, 0x02, 0x2e, 0x75, 0x3b, 0x8b, 0xb4, 0xbc, 0x3b, 0xb7, 0xdb, 0x49, 0x14},
            g2({fp2::fromString("0d4333b77becbf9f9dfa3ca928002233d1ecc854b1447e5a71f751c9042d000f42db91c1d6649a5e0ad22bd7bf7398b8027e4bfada0b47f9f07e04aec463c7371e68f2fd0c738cd517932ea3801a35acf09db018deda57387b0f270f7a219e4d"), fp2::fromString("0cc76dc777ea0d447e02a41004f37a0a7b1fafb6746884e8d9fc276716ccf47e4e0899548a2ec71c2bdf1a2a50e876db053674cba9ef516ddc218fedb37324e6c47de27f88ab7ef123b006127d738293c0277187f7e2f80a299a24d84ed03da7"), fp2::one()})
        },
        pair{
            array<uint8_t, 96>{0x18, 0x70, 0xa7, 0xdb, 0xfd, 0x2a, 0x1d, 0xeb, 0x74, 0x01, 0x5a, 0x35, 0x46, 0xb2, 0x0f, 0x59, 0x80, 0x41, 0xbf, 0x5d, 0x52, 0x02, 0x99, 0x79, 0x56, 0xa9, 0x4a, 0x36, 0x8d, 0x30, 0xd3, 0xf7, 0x0f, 0x18, 0xcd, 0xaa, 0x1d, 0x33, 0xce, 0x97, 0x0a, 0x4e, 0x16, 0xaf, 0x96, 0x1c, 0xbd, 0xcb, 0x04, 0x5a, 0xb3, 0x1c, 0xe4, 0xb5, 0xa8, 0xba, 0x7c, 0x4b, 0x28, 0x51, 0xb6, 0x4f, 0x06, 0x3a, 0x66, 0xcd, 0x12, 0x23, 0xd3, 0xc8, 0x50, 0x05, 0xb7, 0x8e, 0x1b, 0xee, 0xe6, 0x5e, 0x33, 0xc9, 0x0c, 0xee, 0xf0, 0x24, 0x4e, 0x45, 0xfc, 0x45, 0xa5, 0xe1, 0xd6, 0xea, 0xb6, 0x64, 0x4f, 0xdb},
            g2({fp2::fromString("18f0f87b40af67c056915dbaf48534c592524e82c1c2b50c3734d02c0172c80df780a60b5683759298a3303c5d94277809349f1cb5b2e55489dcd45a38545343451cc30a1681c57acd4fb0a6db125f8352c09f4a67eb7d1d8242cb7d3405f97b"), fp2::fromString("10a2ba341bc689ab947b7941ce6ef39be17acaab067bd32bd652b471ab0792c53a2bd03bdac47f96aaafe96e441f63c002f2d9deb2c7742512f5b8230bf0fd83ea42279d7d39779543c1a43b61c885982b611f6a7a24b514995e8a098496b811"), fp2::one()})
        },
        pair{
            array<uint8_t, 96>{0x08, 0x8f, 0xe3, 0x29, 0xb0, 0x54, 0xdb, 0x8a, 0x64, 0x74, 0xf2, 0x1a, 0x7f, 0xbf, 0xdf, 0x17, 0xb4, 0xc1, 0x80, 0x44, 0xdb, 0x29, 0x9d, 0x90, 0x07, 0xaf, 0x58, 0x2c, 0x3d, 0x5f, 0x17, 0xd0, 0x0e, 0x56, 0xd9, 0x99, 0x21, 0xd4, 0xb5, 0x64, 0x0f, 0xce, 0x44, 0xb0, 0x52, 0x19, 0xb5, 0xde, 0x0b, 0x6e, 0x61, 0x35, 0xa4, 0xcd, 0x31, 0xba, 0x98, 0x0d, 0xdb, 0xd1, 0x15, 0xac, 0x48, 0xab, 0xef, 0x7e, 0xc6, 0x0e, 0x22, 0x6f, 0x26, 0x4d, 0x7b, 0xef, 0xe0, 0x02, 0xc1, 0x65, 0xf3, 0xa4, 0x96, 0xf3, 0x6f, 0x76, 0xdd, 0x52, 0x4e, 0xfd, 0x75, 0xd1, 0x74, 0x22, 0x55, 0x8d, 0x10, 0xb4},
            g2({fp2::fromString("19808ec5930a53c7cf5912ccce1cc33f1b3dcff24a53ce1cc4cba41fd6996dbed4843ccdd2eaf6a0cd801e562718d163149fe43777d34f0d25430dea463889bd9393bdfb4932946db23671727081c629ebb98a89604f3433fba1c67d356a4af7"), fp2::fromString("04783e391c30c83f805ca271e353582fdf19d159f6a4c39b73acbb637a9b8ac820cfbe2738d683368a7c07ad020e3e3304c0d6793a766233b2982087b5f4a254f261003ccb3262ea7c50903eecef3e871d1502c293f9e063d7d293f6384f4551"), fp2::one()})
        },
        pair{
            array<uint8_t, 96>{0x03, 0xdf, 0x16, 0xa6, 0x6a, 0x05, 0xe4, 0xc1, 0x18, 0x8c, 0x23, 0x47, 0x88, 0xf4, 0x38, 0x96, 0xe0, 0x56, 0x5b, 0xfb, 0x64, 0xac, 0x49, 0xb9, 0x63, 0x9e, 0x6b, 0x28, 0x4c, 0xc4, 0x7d, 0xad, 0x73, 0xc4, 0x7b, 0xb4, 0xea, 0x7e, 0x67, 0x7d, 0xb8, 0xd4, 0x96, 0xbe, 0xb9, 0x07, 0xfb, 0xb6, 0x0f, 0x45, 0xb5, 0x06, 0x47, 0xd6, 0x74, 0x85, 0x29, 0x5a, 0xa9, 0xeb, 0x2d, 0x91, 0xa8, 0x77, 0xb4, 0x48, 0x13, 0x67, 0x7c, 0x67, 0xc8, 0xd3, 0x5b, 0x21, 0x73, 0xff, 0x3b, 0xa9, 0x5f, 0x7b, 0xd0, 0x80, 0x6f, 0x9c, 0xa8, 0xa1, 0x43, 0x6b, 0x8b, 0x9d, 0x14, 0xee, 0x81, 0xda, 0x4d, 0x7e},
            g2({fp2::fromString("0b8e0094c886487870372eb6264613a6a087c7eb9804fab789be4e47a57b29eb19b1983a51165a1b5eb025865e9fc63a0804152cbf8474669ad7d1796ab92d7ca21f32d8bed70898a748ed4e4e0ec557069003732fc86866d938538a2ae95552"), fp2::fromString("14c80f068ece15a3936bb00c3c883966f75b4e8d9ddde809c11f781ab92d23a2d1d103ad48f6f3bb158bf3e3a406344909e5c8242dd7281ad32c03fe4af3f19167770016255fb25ad9b67ec51d62fade31a1af101e8f6172ec2ee8857662be3a"), fp2::one()})
        },
    };
    for(uint64_t i = 0; i < 5; i++)
    {
        g2 p0 = g2::mapToCurve(fp2::fromBytes(pairs[i].u)).affine();
        if(!p0.equal(pairs[i].expected))
        {
            throw invalid_argument("map to curve fails");
        }
    }
}

void TestG1G2PackUnpack()
{
    for(uint64_t i = 0; i < fuz; i++)
    {
        g1 p = random_g1().affine();
        array<uint8_t, 48> compressed = p.pack();
        g1 q = g1::unpack(compressed);
        if(!p.equal(q))
        {
            throw invalid_argument("G1: packing/unpacking error");
        }
    }
    for(uint64_t i = 0; i < fuz; i++)
    {
        g2 p = random_g2().affine();
        array<uint8_t, 96> compressed = p.pack();
        g2 q = g2::unpack(compressed);
        if(!p.equal(q))
        {
            throw invalid_argument("G2: packing/unpacking error");
        }
    }
}

///////////////////////////////////////////////////////////

void TestPairingExpected()
{
    fp12 expected = fp12::fromString(
        "0f41e58663bf08cf068672cbd01a7ec73baca4d72ca93544deff686bfd6df543d48eaa24afe47e1efde449383b676631" \
        "04c581234d086a9902249b64728ffd21a189e87935a954051c7cdba7b3872629a4fafc05066245cb9108f0242d0fe3ef" \
        "03350f55a7aefcd3c31b4fcb6ce5771cc6a0e9786ab5973320c806ad360829107ba810c5a09ffdd9be2291a0c25a99a2" \
        "11b8b424cd48bf38fcef68083b0b0ec5c81a93b330ee1a677d0d15ff7b984e8978ef48881e32fac91b93b47333e2ba57" \
        "06fba23eb7c5af0d9f80940ca771b6ffd5857baaf222eb95a7d2809d61bfe02e1bfd1b68ff02f0b8102ae1c2d5d5ab1a" \
        "19f26337d205fb469cd6bd15c3d5a04dc88784fbb3d0b2dbdea54d43b2b73f2cbb12d58386a8703e0f948226e47ee89d" \
        "018107154f25a764bd3c79937a45b84546da634b8f6be14a8061e55cceba478b23f7dacaa35c8ca78beae9624045b4b6" \
        "01b2f522473d171391125ba84dc4007cfbf2f8da752f7c74185203fcca589ac719c34dffbbaad8431dad1c1fb597aaa5" \
        "193502b86edb8857c273fa075a50512937e0794e1e65a7617c90d8bd66065b1fffe51d7a579973b1315021ec3c19934f" \
        "1368bb445c7c2d209703f239689ce34c0378a68e72a6b3b216da0e22a5031b54ddff57309396b38c881c4c849ec23e87" \
        "089a1c5b46e5110b86750ec6a532348868a84045483c92b7af5af689452eafabf1a8943e50439f1d59882a98eaa0170f" \
        "1250ebd871fc0a92a7b2d83168d0d727272d441befa15c503dd8e90ce98db3e7b6d194f60839c508a84305aaca1789b6"
    );
    g1 g1One = g1::one();
    g2 g2One = g2::one();
    vector<tuple<g1, g2>> v;
    pairing::addPair(v, g1One, g2One);
    fp12 r = pairing::calculate(v);
    if(!r.equal(expected))
    {
        throw invalid_argument("0: bad pairing");
    }
    if(!r.isGtValid())
    {
        throw invalid_argument("0: element is not in correct subgroup");
    }
}

void TestPairingNonDegeneracy()
{
    g1 g1Zero = g1::zero();
    g2 g2Zero = g2::zero();
    g1 g1One = g1::one();
    g2 g2One = g2::one();
    // e(g1^a, g2^b) != 1
    {
        vector<tuple<g1, g2>> v;
        pairing::addPair(v, g1One, g2One);
        fp12 e = pairing::calculate(v);
        if(e.isOne())
        {
            throw invalid_argument("0: pairing result is not expected to be one");
        }
        if(!e.isGtValid())
        {
            throw invalid_argument("0: pairing result is not valid");
        }
    }
    // e(g1^a, 0) == 1
    {
        vector<tuple<g1, g2>> v;
        pairing::addPair(v, g1One, g2Zero);
        fp12 e = pairing::calculate(v);
        if(!e.isOne())
        {
            throw invalid_argument("1: pairing result is expected to be one");
        }
    }
    // e(0, g2^b) == 1
    {
        vector<tuple<g1, g2>> v;
        pairing::addPair(v, g1Zero, g2One);
        fp12 e = pairing::calculate(v);
        if(!e.isOne())
        {
            throw invalid_argument("2: pairing result is expected to be one");
        }
    }
    //
    {
        vector<tuple<g1, g2>> v;
        pairing::addPair(v, g1Zero, g2One);
        pairing::addPair(v, g1One, g2Zero);
        pairing::addPair(v, g1Zero, g2Zero);
        fp12 e = pairing::calculate(v);
        if(!e.isOne())
        {
            throw invalid_argument("3: pairing result is expected to be one");
        }
    }
    //
    {
        fp12 expected = fp12::fromString(
            "0f41e58663bf08cf068672cbd01a7ec73baca4d72ca93544deff686bfd6df543d48eaa24afe47e1efde449383b676631" \
            "04c581234d086a9902249b64728ffd21a189e87935a954051c7cdba7b3872629a4fafc05066245cb9108f0242d0fe3ef" \
            "03350f55a7aefcd3c31b4fcb6ce5771cc6a0e9786ab5973320c806ad360829107ba810c5a09ffdd9be2291a0c25a99a2" \
            "11b8b424cd48bf38fcef68083b0b0ec5c81a93b330ee1a677d0d15ff7b984e8978ef48881e32fac91b93b47333e2ba57" \
            "06fba23eb7c5af0d9f80940ca771b6ffd5857baaf222eb95a7d2809d61bfe02e1bfd1b68ff02f0b8102ae1c2d5d5ab1a" \
            "19f26337d205fb469cd6bd15c3d5a04dc88784fbb3d0b2dbdea54d43b2b73f2cbb12d58386a8703e0f948226e47ee89d" \
            "018107154f25a764bd3c79937a45b84546da634b8f6be14a8061e55cceba478b23f7dacaa35c8ca78beae9624045b4b6" \
            "01b2f522473d171391125ba84dc4007cfbf2f8da752f7c74185203fcca589ac719c34dffbbaad8431dad1c1fb597aaa5" \
            "193502b86edb8857c273fa075a50512937e0794e1e65a7617c90d8bd66065b1fffe51d7a579973b1315021ec3c19934f" \
            "1368bb445c7c2d209703f239689ce34c0378a68e72a6b3b216da0e22a5031b54ddff57309396b38c881c4c849ec23e87" \
            "089a1c5b46e5110b86750ec6a532348868a84045483c92b7af5af689452eafabf1a8943e50439f1d59882a98eaa0170f" \
            "1250ebd871fc0a92a7b2d83168d0d727272d441befa15c503dd8e90ce98db3e7b6d194f60839c508a84305aaca1789b6"
        );
        vector<tuple<g1, g2>> v;
        pairing::addPair(v, g1Zero, g2One);
        pairing::addPair(v, g1One, g2Zero);
        pairing::addPair(v, g1Zero, g2Zero);
        pairing::addPair(v, g1One, g2One);
        fp12 e = pairing::calculate(v);
        if(!e.equal(expected))
        {
            throw invalid_argument("4: bad pairing");
        }
    }
}

void TestPairingBilinearity()
{
    // e(a*G1, b*G2) = e(G1, G2)^c
    {
        array<uint64_t, 4> a = {17, 0, 0, 0};
        array<uint64_t, 4> b = {117, 0, 0, 0};
        // c = a * b = 1989
        array<uint64_t, 4> c = {1989, 0, 0, 0};
        g1 G1 = g1::one();
        g2 G2 = g2::one();
        vector<tuple<g1, g2>> v;
        pairing::addPair(v, G1, G2);
        fp12 e0 = pairing::calculate(v);
        g1 P1 = G1.mulScalar(a);
        g2 P2 = G2.mulScalar(b);
        v = {};
        pairing::addPair(v, P1, P2);
        fp12 e1 = pairing::calculate(v);
        e0 = e0.cyclotomicExp(c);
        if(!e0.equal(e1))
        {
            throw invalid_argument("bad pairing, 1");
        }
    }
    // e(a * G1, b * G2) = e((a + b) * G1, G2)
    {
        // scalars
        array<uint64_t, 4> a = {17, 0, 0, 0};
        array<uint64_t, 4> b = {117, 0, 0, 0};
        // c = a * b = 1989
        array<uint64_t, 4> c = {1989, 0, 0, 0};
        vector<tuple<g1, g2>> v;
        // LHS
        g1 G1 = g1::one();
        g2 G2 = g2::one();
        G1 = G1.mulScalar(c);
        pairing::addPair(v, G1, G2);
        // RHS
        g1 P1 = g1::one();
        g2 P2 = g2::one();
        P1 = P1.mulScalar(a);
        P2 = P2.mulScalar(b);
        P1 = P1.neg();
        pairing::addPair(v, P1, P2);
        // should be one
        if(!pairing::calculate(v).isOne())
        {
            throw invalid_argument("bad pairing, 2");
        }
    }
    // e(a * G1, b * G2) = e((a + b) * G1, G2)
    {
        // scalars
        array<uint64_t, 4> a = {17, 0, 0, 0};
        array<uint64_t, 4> b = {117, 0, 0, 0};
        // c = a * b = 1989
        array<uint64_t, 4> c = {1989, 0, 0, 0};
        vector<tuple<g1, g2>> v;
        // LHS
        g1 G1 = g1::one();
        g2 G2 = g2::one();
        G2 = G2.mulScalar(c);
        pairing::addPair(v, G1, G2);
        // RHS
        g1 H1 = g1::one();
        g2 H2 = g2::one();
        H1 = H1.mulScalar(a);
        H2 = H2.mulScalar(b);
        H1 = H1.neg();
        pairing::addPair(v, H1, H2);
        // should be one
        if(!pairing::calculate(v).isOne())
        {
            throw invalid_argument("bad pairing, 3");
        }
    }
}

void TestPairingMulti()
{
    // e(G1, G2) ^ t == e(a01 * G1, a02 * G2) * e(a11 * G1, a12 * G2) * ... * e(an1 * G1, an2 * G2)
    // where t = sum(ai1 * ai2)
    uint64_t numOfPair = 100;
    vector<tuple<g1, g2>> v;
    array<uint64_t, 10> targetExp = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    // RHS
    for(uint64_t i = 0; i < numOfPair; i++)
    {
        // (ai1 * G1, ai2 * G2)
        array<uint64_t, 4> a1 = random_scalar();
        array<uint64_t, 4> a2 = random_scalar();
        g1 P1 = g1::one();
        g2 P2 = g2::one();
        P1 = P1.mulScalar(a1);
        P2 = P2.mulScalar(a2);
        pairing::addPair(v, P1, P2);
        // accumulate targetExp
        // t += (ai1 * ai2)
        array<uint64_t, 10> tmp = mul<10, 4, 4>(a1, a2);
        targetExp = add<10, 10, 10>(targetExp, tmp);
    }
    // LHS
    // e(t * G1, G2)
    g1 T1 = g1::one();
    g2 T2 = g2::one();
    T1 = T1.mulScalar(targetExp);
    T1 = T1.neg();
    pairing::addPair(v, T1, T2);
    if(!pairing::calculate(v).isOne())
    {
        throw invalid_argument("fail multi pairing");
    }
}

///////////////////////////////////////////////////////////

void TestEIP2333(string seedHex, string masterSkHex, string childSkHex, uint32_t childIndex)
{
    vector<uint8_t> masterSk = HexToBytes(masterSkHex);
    vector<uint8_t> childSk = HexToBytes(childSkHex);

    array<uint64_t, 4> master = secret_key(HexToBytes(seedHex));
    array<uint64_t, 4> child = derive_child_sk(master, childIndex);

    array<uint8_t, 32> calculatedMaster = toBEBytes(master);
    array<uint8_t, 32> calculatedChild = toBEBytes(child);

    if(calculatedMaster.size() != 32)
    {
        throw invalid_argument("TestEIP2333: calculatedMaster size invalid!");
    }
    if(calculatedChild.size() != 32)
    {
        throw invalid_argument("TestEIP2333: calculatedChild size invalid!");
    }
    for(int i=0; i<32; i++)
    {
        if(calculatedMaster[i] != masterSk[i])
        {
            throw invalid_argument("TestEIP2333: calculatedMaster != masterSk");
        }
    }
    for (int i=0; i<32; i++)
    {
        if(calculatedChild[i] != childSk[i])
        {
            throw invalid_argument("TestEIP2333: calculatedChild != childSk");
        }
    }
}

void TestsEIP2333()
{
    // The comments in the test cases correspond to  integers that are converted to
    // bytes using python int.to_bytes(32, "big").hex(), since the EIP spec provides ints, but c++
    // does not support bigint by default
    {
        TestEIP2333("3141592653589793238462643383279502884197169399375105820974944592",
                    // 36167147331491996618072159372207345412841461318189449162487002442599770291484
                    "4ff5e145590ed7b71e577bb04032396d1619ff41cb4e350053ed2dce8d1efd1c",
                    // 41787458189896526028601807066547832426569899195138584349427756863968330588237
                    "5c62dcf9654481292aafa3348f1d1b0017bbfb44d6881d26d2b17836b38f204d",
                    3141592653
        );
    }
    {
        TestEIP2333("0x0099FF991111002299DD7744EE3355BBDD8844115566CC55663355668888CC00",
                    // 13904094584487173309420026178174172335998687531503061311232927109397516192843
                    "1ebd704b86732c3f05f30563dee6189838e73998ebc9c209ccff422adee10c4b",
                    // 12482522899285304316694838079579801944734479969002030150864436005368716366140
                    "1b98db8b24296038eae3f64c25d693a269ef1e4d7ae0f691c572a46cf3c0913c",
                    4294967295
        );
    }
    {
        TestEIP2333("0xd4e56740f876aef8c010b86a40d5f56745a118d0906a34e69aec8c0db1cb8fa3",
                    // 44010626067374404458092393860968061149521094673473131545188652121635313364506
                    "614d21b10c0e4996ac0608e0e7452d5720d95d20fe03c59a3321000a42432e1a",
                    // 4011524214304750350566588165922015929937602165683407445189263506512578573606
                    "08de7136e4afc56ae3ec03b20517d9c1232705a747f588fd17832f36ae337526",
                    42
        );
    }
    {
        TestEIP2333("c55257c360c07c72029aebc1b53c05ed0362ada38ead3e3e9efa3708e53495531f09a6987599d18264c1e1c92f2cf141630c7a3c4ab7c81b2f001698e7463b04",
                    // 5399117110774477986698372024995405256382522670366369834617409486544348441851
                    "0x0befcabff4a664461cc8f190cdd51c05621eb2837c71a1362df5b465a674ecfb",
                    // 11812940737387919040225825939013910852517748782307378293770044673328955938106
                    "1a1de3346883401f1e3b2281be5774080edb8e5ebe6f776b0f7af9fea942553a",
                    0
        );
    }
}

void TestUnhardenedHDKeys()
{
    {
        const vector<uint8_t> seed = {1, 50, 6, 244, 24, 199, 1, 25, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29};

        array<uint64_t, 4> sk = secret_key(seed);
        g1 pk = public_key(sk);

        array<uint64_t, 4> childSk = derive_child_sk_unhardened(sk, 42);
        g1 childPk = derive_shild_g1_unhardened(pk, 42);

        if(!public_key(childSk).equal(childPk))
        {
            throw invalid_argument("error");
        }

        array<uint64_t, 4> grandchildSk = derive_child_sk_unhardened(childSk, 12142);
        g1 grandcihldPk = derive_shild_g1_unhardened(childPk, 12142);

        if(!public_key(grandchildSk).equal(grandcihldPk))
        {
            throw invalid_argument("error");
        }
    }
    {
        const vector<uint8_t> seed = {2, 50, 6, 244, 24, 199, 1, 25, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29};

        array<uint64_t, 4> sk = secret_key(seed);
        g1 pk = public_key(sk);

        array<uint64_t, 4> childSk = derive_child_sk_unhardened(sk, 42);
        g1 childPk = derive_shild_g1_unhardened(pk, 42);

        array<uint64_t, 4> childSkHardened = derive_child_sk(sk, 42);
        if(!public_key(childSk).equal(childPk))
        {
            throw invalid_argument("SkToG1(childSk) != childPk");
        }
        if(childSkHardened == childSk)
        {
            throw invalid_argument("childSkHardened == childSk");
        }
        if(public_key(childSkHardened).equal(childPk))
        {
            throw invalid_argument("SkToG1(childSkHardened) == childPk");
        }
    }
}

void TestIETFVectors()
{
    string sig1BasicHex = "b6ba34fac33c7f129d602a0bc8a3d43f9abc014eceaab7359146b4b150e57b808645738f35671e9e10e0d862a30cab70074eb5831d13e6a5b162d01eebe687d0164adbd0a864370a7c222a2768d7704da254f1bf1823665bc2361f9dd8c00e99";
    string sk = "0x0101010101010101010101010101010101010101010101010101010101010101";
    vector<uint8_t> msg = {3, 1, 4, 1, 5, 9};
    array<uint64_t, 4UL> skobj = sk_from_bytes(*reinterpret_cast<const array<uint8_t, 32>*>(HexToBytes(sk).data()), false);
    g2 sig = sign(skobj, msg).affine();
    g2 sig1 = g2::unpack(*reinterpret_cast<const array<uint8_t, 96>*>(HexToBytes(sig1BasicHex).data()));
    
    array<uint8_t, 96> bytes = sig.pack();
    string s = BytesToHex(vector<uint8_t>(bytes.begin(), bytes.end()));

    if(!sig.affine().equal(sig1))
    {
        throw invalid_argument("signatures must be equal");
    }
}

void TestChiaVectors()
{
    vector<uint8_t> seed1(32, 0x00);  // All 0s
    vector<uint8_t> seed2(32, 0x01);  // All 1s
    vector<uint8_t> message1 = {7, 8, 9};
    vector<uint8_t> message2 = {10, 11, 12};

    array<uint64_t, 4> sk1 = secret_key(seed1);
    g1 pk1 = public_key(sk1);
    g2 sig1 = sign(sk1, message1);
    if(!verify(pk1, message1, sig1))
    {
        throw invalid_argument("sig1 verification failed");
    }

    array<uint64_t, 4> sk2 = secret_key(seed2);
    g1 pk2 = public_key(sk2);
    g2 sig2 = sign(sk2, message2);
    if(!verify(pk2, message2, sig2))
    {
        throw invalid_argument("sig2 verification failed");
    }

    g2 aggSig1 = aggregate_signatures({sig1, sig2});
    if(!aggregate_verify({pk1, pk2}, {message1, message2}, aggSig1))
    {
        throw invalid_argument("aggSig1 verification failed");
    }

    vector<uint8_t> message3 = {1, 2, 3};
    vector<uint8_t> message4 = {1, 2, 3, 4};
    vector<uint8_t> message5 = {1, 2};

    g2 sig3 = sign(sk1, message3);
    g2 sig4 = sign(sk1, message4);
    g2 sig5 = sign(sk2, message5);

    g2 aggSig2 = aggregate_signatures({sig3, sig4, sig5});

    if(!aggregate_verify({pk1, pk1, pk2}, vector<vector<uint8_t>>{message3, message4, message5}, aggSig2))
    {
        throw invalid_argument("aggSig2 verification failed");
    }
}

void TestChiaVectors2()
{
    vector<uint8_t> message1 = {1, 2, 3, 40};
    vector<uint8_t> message2 = {5, 6, 70, 201};
    vector<uint8_t> message3 = {9, 10, 11, 12, 13};
    vector<uint8_t> message4 = {15, 63, 244, 92, 0, 1};

    vector<uint8_t> seed1(32, 0x02);  // All 2s
    vector<uint8_t> seed2(32, 0x03);  // All 3s

    array<uint64_t, 4> sk1 = secret_key(seed1);
    array<uint64_t, 4> sk2 = secret_key(seed2);

    g1 pk1 = public_key(sk1);
    g1 pk2 = public_key(sk2);

    g2 sig1 = sign(sk1, message1);
    g2 sig2 = sign(sk2, message2);
    g2 sig3 = sign(sk2, message1);
    g2 sig4 = sign(sk1, message3);
    g2 sig5 = sign(sk1, message1);
    g2 sig6 = sign(sk1, message4);


    g2 aggSigL = aggregate_signatures({sig1, sig2});
    g2 aggSigR = aggregate_signatures({sig3, sig4, sig5});
    g2 aggSig = aggregate_signatures({aggSigL, aggSigR, sig6});

    if(!aggregate_verify({pk1, pk2, pk2, pk1, pk1, pk1}, vector<vector<uint8_t>>{message1, message2, message1, message3, message1, message4}, aggSig))
    {
        throw invalid_argument("aggregate verify error");
    }
}

int main()
{
    
    TestFieldElementValidation();
    TestFieldElementEquality();
    TestFieldElementHelpers();
    TestFieldElementSerialization();
    TestFieldElementByteInputs();

    TestG1Serialization();
    TestG1IsOnCurve();
    TestG1AdditiveProperties();
    TestG1MultiplicativePropertiesExpected();
    TestG1MultiplicativeProperties();
    TestG1MultiExpExpected();
    TestG1MultiExpBatch();
    TestG1MapToCurve();

    TestG2Serialization();
    TestG2IsOnCurve();
    TestG2AdditiveProperties();
    TestG2MultiplicativeProperties();
    TestG2MultiExpExpected();
    TestG2MultiExpBatch();
    TestG2MapToCurve();
    TestG1G2PackUnpack();

    TestPairingExpected();
    TestPairingNonDegeneracy();
    TestPairingBilinearity();
    TestPairingMulti();

    TestsEIP2333();
    TestUnhardenedHDKeys();
    TestIETFVectors();
    TestChiaVectors();
    TestChiaVectors2();
    
    return 0;
}