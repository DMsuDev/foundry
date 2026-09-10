// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#include "foundry/types/bitmask.h"
#include "foundry/types/flags.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <type_traits>

// =============================================================================
// Test enums
// =============================================================================

enum class Perm : uint32_t {
    None    = 0,
    Read    = FOUNDRY_BIT(0),
    Write   = FOUNDRY_BIT(1),
    Execute = FOUNDRY_BIT(2),
};
FOUNDRY_DECLARE_FLAGS(Perm)

enum class Wide : uint64_t {
    None  = 0,
    Alpha = FOUNDRY_BIT64(0),
    Beta  = FOUNDRY_BIT64(32),
    Gamma = FOUNDRY_BIT64(63),
};
FOUNDRY_DECLARE_FLAGS(Wide)

using foundry::types::Flags;

// =============================================================================
// Fixture
// =============================================================================

class FlagsTest : public ::testing::Test {
protected:
    Flags<Perm> empty{};
    Flags<Perm> readOnly{ Perm::Read };
    Flags<Perm> readWrite{ Perm::Read | Perm::Write };
    Flags<Perm> all{ Perm::Read | Perm::Write | Perm::Execute };
};

// =============================================================================
// Construction
// =============================================================================

TEST_F(FlagsTest, DefaultConstructedIsEmpty)
{
    EXPECT_EQ(empty.bits(), 0u);
    EXPECT_TRUE(empty.none());
    EXPECT_FALSE(empty.any());
}

TEST_F(FlagsTest, ConstructFromSingleEnumerator)
{
    EXPECT_TRUE(readOnly.has(Perm::Read));
    EXPECT_FALSE(readOnly.has(Perm::Write));
    EXPECT_FALSE(readOnly.has(Perm::Execute));
}

TEST_F(FlagsTest, ConstructFromCombinedEnumerators)
{
    EXPECT_TRUE(readWrite.has(Perm::Read));
    EXPECT_TRUE(readWrite.has(Perm::Write));
    EXPECT_FALSE(readWrite.has(Perm::Execute));
}

// =============================================================================
// Underlying type
// =============================================================================

TEST_F(FlagsTest, UnderlyingTypeMatchesEnum)
{
    static_assert(std::is_same_v<Flags<Perm>::underlying_type, uint32_t>);
    static_assert(std::is_same_v<Flags<Wide>::underlying_type, uint64_t>);
}

// =============================================================================
// Query — has(E) and has(Flags<E>)
// =============================================================================

TEST_F(FlagsTest, HasSingleEnumerator)
{
    EXPECT_TRUE(readWrite.has(Perm::Read));
    EXPECT_TRUE(readWrite.has(Perm::Write));
    EXPECT_FALSE(readWrite.has(Perm::Execute));
    EXPECT_FALSE(empty.has(Perm::Read));
}

TEST_F(FlagsTest, HasCombinedMask)
{
    EXPECT_TRUE(readWrite.has(Perm::Read | Perm::Write));
    EXPECT_FALSE(readWrite.has(Perm::Read | Perm::Execute));
    EXPECT_FALSE(readWrite.has(Perm::Write | Perm::Execute));
}

TEST_F(FlagsTest, HasEmptyMaskAlwaysReturnsTrue)
{
    // An empty mask is trivially a subset of any set.
    EXPECT_TRUE(readWrite.has(empty));
    EXPECT_TRUE(empty.has(empty));
    EXPECT_TRUE(all.has(empty));
}

// =============================================================================
// Query — any / none
// =============================================================================

TEST_F(FlagsTest, AnyAndNone)
{
    EXPECT_FALSE(empty.any());
    EXPECT_TRUE(empty.none());

    EXPECT_TRUE(readOnly.any());
    EXPECT_FALSE(readOnly.none());

    EXPECT_TRUE(all.any());
    EXPECT_FALSE(all.none());
}

// =============================================================================
// Mutation — set
// =============================================================================

TEST_F(FlagsTest, SetSingleEnumerator)
{
    Flags<Perm> f{};
    f.set(Perm::Read);
    EXPECT_TRUE(f.has(Perm::Read));
    EXPECT_FALSE(f.has(Perm::Write));
}

TEST_F(FlagsTest, SetCombinedMask)
{
    Flags<Perm> f{};
    f.set(Perm::Read | Perm::Write);
    EXPECT_TRUE(f.has(Perm::Read));
    EXPECT_TRUE(f.has(Perm::Write));
    EXPECT_FALSE(f.has(Perm::Execute));
}

TEST_F(FlagsTest, SetIsIdempotent)
{
    Flags<Perm> f{ Perm::Read };
    f.set(Perm::Read);
    EXPECT_EQ(f.bits(), static_cast<uint32_t>(Perm::Read));
}

// =============================================================================
// Mutation — unset
// =============================================================================

TEST_F(FlagsTest, UnsetSingleEnumerator)
{
    Flags<Perm> f = all;
    f.unset(Perm::Write);
    EXPECT_FALSE(f.has(Perm::Write));
    EXPECT_TRUE(f.has(Perm::Read));
    EXPECT_TRUE(f.has(Perm::Execute));
}

TEST_F(FlagsTest, UnsetCombinedMask)
{
    Flags<Perm> f = all;
    f.unset(Perm::Read | Perm::Write);
    EXPECT_FALSE(f.has(Perm::Read));
    EXPECT_FALSE(f.has(Perm::Write));
    EXPECT_TRUE(f.has(Perm::Execute));
}

TEST_F(FlagsTest, UnsetNotSetBitIsNoOp)
{
    Flags<Perm> f{ Perm::Read };
    f.unset(Perm::Execute);
    EXPECT_EQ(f.bits(), static_cast<uint32_t>(Perm::Read));
}

// =============================================================================
// Mutation — toggle
// =============================================================================

TEST_F(FlagsTest, ToggleSingleEnumeratorFlipsBothWays)
{
    Flags<Perm> f{ Perm::Read };
    f.toggle(Perm::Read);
    EXPECT_FALSE(f.has(Perm::Read));
    f.toggle(Perm::Read);
    EXPECT_TRUE(f.has(Perm::Read));
}

TEST_F(FlagsTest, ToggleCombinedMask)
{
    Flags<Perm> f = readWrite;
    f.toggle(Perm::Read | Perm::Execute);
    EXPECT_FALSE(f.has(Perm::Read));
    EXPECT_TRUE(f.has(Perm::Write));
    EXPECT_TRUE(f.has(Perm::Execute));
}

// =============================================================================
// Mutation — clear
// =============================================================================

TEST_F(FlagsTest, ClearResetsAllBitsToZero)
{
    Flags<Perm> f = all;
    f.clear();
    EXPECT_EQ(f.bits(), 0u);
    EXPECT_TRUE(f.none());
}

// =============================================================================
// Bitwise operators — binary
// =============================================================================

TEST_F(FlagsTest, OperatorOr)
{
    auto result = readOnly | Flags<Perm>{ Perm::Write };
    EXPECT_TRUE(result.has(Perm::Read));
    EXPECT_TRUE(result.has(Perm::Write));
    EXPECT_EQ(result, readWrite);
    // Identity: f | empty == f
    EXPECT_EQ(readWrite | empty, readWrite);
}

TEST_F(FlagsTest, OperatorAnd)
{
    auto result = all & readWrite;
    EXPECT_TRUE(result.has(Perm::Read));
    EXPECT_TRUE(result.has(Perm::Write));
    EXPECT_FALSE(result.has(Perm::Execute));
    // Disjoint sets produce empty
    EXPECT_TRUE((readOnly & Flags<Perm>{ Perm::Execute }).none());
    // Identity: f & f == f
    EXPECT_EQ(readWrite & readWrite, readWrite);
}

TEST_F(FlagsTest, OperatorXor)
{
    auto result = readWrite ^ Flags<Perm>{ Perm::Write | Perm::Execute };
    EXPECT_TRUE(result.has(Perm::Read));
    EXPECT_FALSE(result.has(Perm::Write));
    EXPECT_TRUE(result.has(Perm::Execute));
    // f ^ f == empty
    EXPECT_TRUE((readWrite ^ readWrite).none());
    // f ^ empty == f
    EXPECT_EQ(readWrite ^ empty, readWrite);
}

// =============================================================================
// Bitwise operators — complement
// =============================================================================

TEST_F(FlagsTest, OperatorComplementInvertsAllBits)
{
    // ~Read flips all 32 bits, including undefined ones.
    EXPECT_EQ((~readOnly).bits(), ~static_cast<uint32_t>(Perm::Read));
    // ~empty == all-ones
    EXPECT_EQ((~empty).bits(), UINT32_MAX);
    // Double complement is identity
    EXPECT_EQ((~~readWrite).bits(), readWrite.bits());
}

TEST_F(FlagsTest, FreeFunctionComplementOnEnumerator)
{
    auto result = ~Perm::Read;
    static_assert(std::is_same_v<decltype(result), Flags<Perm>>);
    EXPECT_EQ(result.bits(), ~static_cast<uint32_t>(Perm::Read));
}

// =============================================================================
// Bitwise assignment operators — with Flags<E> and E
// =============================================================================

TEST_F(FlagsTest, OperatorOrAssign)
{
    Flags<Perm> f{ Perm::Read };
    f |= Flags<Perm>{ Perm::Write };
    EXPECT_EQ(f, readWrite);
}

TEST_F(FlagsTest, OperatorAndAssign)
{
    Flags<Perm> f = all;
    f &= readWrite;
    EXPECT_EQ(f, readWrite);
}

TEST_F(FlagsTest, OperatorXorAssign)
{
    Flags<Perm> f = readWrite;
    f ^= Flags<Perm>{ Perm::Write };
    EXPECT_EQ(f, readOnly);
}

// =============================================================================
// Free-function operators (E op E, E op Flags<E>, Flags<E> op E)
// =============================================================================

TEST_F(FlagsTest, FreeFunctionEnumOpEnum)
{
    auto orResult  = Perm::Read | Perm::Write;
    auto andResult = Perm::Read & Perm::Read;
    auto xorResult = Perm::Read ^ Perm::Read;

    static_assert(std::is_same_v<decltype(orResult),  Flags<Perm>>);
    static_assert(std::is_same_v<decltype(andResult), Flags<Perm>>);
    static_assert(std::is_same_v<decltype(xorResult), Flags<Perm>>);

    EXPECT_EQ(orResult,  readWrite);
    EXPECT_TRUE(andResult.has(Perm::Read));
    EXPECT_TRUE(xorResult.none());
}

TEST_F(FlagsTest, FreeFunctionEnumOrFlags)
{
    auto result = Perm::Read | Flags<Perm>{ Perm::Write };
    static_assert(std::is_same_v<decltype(result), Flags<Perm>>);
    EXPECT_EQ(result, readWrite);
}

TEST_F(FlagsTest, FreeFunctionEnumAndFlags)
{
    auto result = Perm::Read & Flags<Perm>{ Perm::Read | Perm::Write };
    static_assert(std::is_same_v<decltype(result), Flags<Perm>>);
    EXPECT_EQ(result, readOnly);
}

// =============================================================================
// Comparison
// =============================================================================

TEST_F(FlagsTest, Equality)
{
    EXPECT_EQ(empty, Flags<Perm>{});
    EXPECT_EQ(readWrite, Perm::Read | Perm::Write);
    EXPECT_NE(readOnly, readWrite);
    EXPECT_NE(readWrite, all);
}

// =============================================================================
// operator bool
// =============================================================================

TEST_F(FlagsTest, OperatorBool)
{
    EXPECT_FALSE(static_cast<bool>(empty));
    EXPECT_TRUE(static_cast<bool>(readOnly));
    // Usable in if-condition directly
    if (readWrite) { SUCCEED(); }
    else           { FAIL() << "readWrite should be truthy"; }
}

// =============================================================================
// Raw bits
// =============================================================================

TEST_F(FlagsTest, BitsReturnsExactUnderlyingValue)
{
    EXPECT_EQ(empty.bits(),     0u);
    EXPECT_EQ(readOnly.bits(),  static_cast<uint32_t>(Perm::Read));
    EXPECT_EQ(readWrite.bits(), static_cast<uint32_t>(Perm::Read)
                              | static_cast<uint32_t>(Perm::Write));
    EXPECT_EQ(all.bits(),       static_cast<uint32_t>(Perm::Read)
                              | static_cast<uint32_t>(Perm::Write)
                              | static_cast<uint32_t>(Perm::Execute));
}

// =============================================================================
// 64-bit flags
// =============================================================================

TEST(FlagsWideTest, BasicOperations)
{
    Flags<Wide> f = Wide::Alpha | Wide::Gamma;

    EXPECT_TRUE(f.has(Wide::Alpha));
    EXPECT_FALSE(f.has(Wide::Beta));
    EXPECT_TRUE(f.has(Wide::Gamma));

    f.set(Wide::Beta);
    EXPECT_TRUE(f.has(Wide::Beta));

    f.unset(Wide::Gamma);
    EXPECT_FALSE(f.has(Wide::Gamma));
}

TEST(FlagsWideTest, HighBitRoundtrips)
{
    Flags<Wide> f{ Wide::Gamma };
    EXPECT_EQ(f.bits(), static_cast<uint64_t>(Wide::Gamma));
    EXPECT_EQ(f.bits(), 1ull << 63);
}

TEST(FlagsWideTest, ComplementInverts64Bits)
{
    Flags<Wide> f{ Wide::Alpha };
    EXPECT_EQ((~f).bits(), ~static_cast<uint64_t>(Wide::Alpha));
    EXPECT_EQ((~~f).bits(), f.bits());
}

// =============================================================================
// Bitmask macros
// =============================================================================

TEST(BitmaskTest, FoundryBit)
{
    EXPECT_EQ(FOUNDRY_BIT(0),  0x00000001u);
    EXPECT_EQ(FOUNDRY_BIT(1),  0x00000002u);
    EXPECT_EQ(FOUNDRY_BIT(4),  0x00000010u);
    EXPECT_EQ(FOUNDRY_BIT(31), 0x80000000u);
}

TEST(BitmaskTest, FoundryBit64)
{
    EXPECT_EQ(FOUNDRY_BIT64(0),  1ull << 0);
    EXPECT_EQ(FOUNDRY_BIT64(1),  1ull << 1);
    EXPECT_EQ(FOUNDRY_BIT64(32), 1ull << 32);
    EXPECT_EQ(FOUNDRY_BIT64(63), 1ull << 63);
}
