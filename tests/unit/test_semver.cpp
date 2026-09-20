// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#include "foundry/types/semantic_version.h"

#include <gtest/gtest.h>

// ============================================================================
// Construction
// ============================================================================

TEST(SemanticVersionTest, ConstructsFromComponents)
{
    const foundry::types::SemanticVersion v{1, 2, 3};

    EXPECT_EQ(v.major(), 1u);
    EXPECT_EQ(v.minor(), 2u);
    EXPECT_EQ(v.patch(), 3u);
    EXPECT_EQ(v.prerelease(), "");
    EXPECT_EQ(v.build(), "");
    EXPECT_FALSE(v.is_prerelease());
    EXPECT_FALSE(v.has_build_metadata());
}

TEST(SemanticVersionTest, ConstructsWithPrerelease)
{
    const foundry::types::SemanticVersion v{1, 2, 3, "alpha.1"};

    EXPECT_EQ(v.prerelease(), "alpha.1");
    EXPECT_TRUE(v.is_prerelease());
    EXPECT_FALSE(v.has_build_metadata());
}

TEST(SemanticVersionTest, ConstructsWithBuildMetadata)
{
    const foundry::types::SemanticVersion v{1, 2, 3, "", "build.42"};

    EXPECT_EQ(v.build(), "build.42");
    EXPECT_FALSE(v.is_prerelease());
    EXPECT_TRUE(v.has_build_metadata());
}

TEST(SemanticVersionTest, ConstructsWithPrereleaseAndBuild)
{
    const foundry::types::SemanticVersion v{1, 2, 3, "rc.1", "build.42"};

    EXPECT_EQ(v.prerelease(), "rc.1");
    EXPECT_EQ(v.build(), "build.42");
    EXPECT_TRUE(v.is_prerelease());
    EXPECT_TRUE(v.has_build_metadata());
}

// ============================================================================
// Parsing — Valid Inputs
// ============================================================================

TEST(SemanticVersionTest, ParsesMinimalVersion)
{
    const auto v = foundry::types::SemanticVersion::parse("0.0.0");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->major(), 0u);
    EXPECT_EQ(v->minor(), 0u);
    EXPECT_EQ(v->patch(), 0u);
}

TEST(SemanticVersionTest, ParsesCoreVersion)
{
    const auto v = foundry::types::SemanticVersion::parse("1.2.3");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->major(), 1u);
    EXPECT_EQ(v->minor(), 2u);
    EXPECT_EQ(v->patch(), 3u);
    EXPECT_FALSE(v->is_prerelease());
    EXPECT_FALSE(v->has_build_metadata());
}

TEST(SemanticVersionTest, ParsesPrereleaseAlpha)
{
    const auto v = foundry::types::SemanticVersion::parse("1.2.3-alpha");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->prerelease(), "alpha");
    EXPECT_TRUE(v->is_prerelease());
}

TEST(SemanticVersionTest, ParsesPrereleaseAlphaDotNumeric)
{
    const auto v = foundry::types::SemanticVersion::parse("1.2.3-alpha.1");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->prerelease(), "alpha.1");
}

TEST(SemanticVersionTest, ParsesPrereleaseAlphaDotAlpha)
{
    const auto v = foundry::types::SemanticVersion::parse("1.2.3-alpha.beta");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->prerelease(), "alpha.beta");
}

TEST(SemanticVersionTest, ParsesPrereleaseBeta)
{
    const auto v = foundry::types::SemanticVersion::parse("1.2.3-beta");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->prerelease(), "beta");
}

TEST(SemanticVersionTest, ParsesPrereleaseBetaDot2)
{
    const auto v = foundry::types::SemanticVersion::parse("1.2.3-beta.2");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->prerelease(), "beta.2");
}

TEST(SemanticVersionTest, ParsesPrereleaseBetaDot11)
{
    const auto v = foundry::types::SemanticVersion::parse("1.2.3-beta.11");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->prerelease(), "beta.11");
}

TEST(SemanticVersionTest, ParsesPrereleaseRcDot1)
{
    const auto v = foundry::types::SemanticVersion::parse("1.2.3-rc.1");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->prerelease(), "rc.1");
}

TEST(SemanticVersionTest, ParsesBuildMetadataOnly)
{
    const auto v = foundry::types::SemanticVersion::parse("1.2.3+build.42");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->build(), "build.42");
    EXPECT_FALSE(v->is_prerelease());
    EXPECT_TRUE(v->has_build_metadata());
}

TEST(SemanticVersionTest, ParsesPrereleaseAndBuild)
{
    const auto v = foundry::types::SemanticVersion::parse("1.2.3-alpha.1+build.42");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->prerelease(), "alpha.1");
    EXPECT_EQ(v->build(), "build.42");
}

TEST(SemanticVersionTest, ParsesBuildMetadataWithLeadingZeroes)
{
    // Leading zeroes are allowed in build metadata identifiers.
    const auto v = foundry::types::SemanticVersion::parse("1.2.3+build.001");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->build(), "build.001");
}

// ============================================================================
// Parsing — Invalid Inputs
// ============================================================================

TEST(SemanticVersionTest, RejectsEmpty)
{
    EXPECT_FALSE(foundry::types::SemanticVersion::parse("").has_value());
}

TEST(SemanticVersionTest, RejectsLeadingZeroInMajor)
{
    EXPECT_FALSE(foundry::types::SemanticVersion::parse("01.2.3").has_value());
}

TEST(SemanticVersionTest, RejectsLeadingZeroInMinor)
{
    EXPECT_FALSE(foundry::types::SemanticVersion::parse("1.02.3").has_value());
}

TEST(SemanticVersionTest, RejectsLeadingZeroInPatch)
{
    EXPECT_FALSE(foundry::types::SemanticVersion::parse("1.2.03").has_value());
}

TEST(SemanticVersionTest, RejectsMissingPatch)
{
    EXPECT_FALSE(foundry::types::SemanticVersion::parse("1.2").has_value());
}

TEST(SemanticVersionTest, RejectsTrailingDotOnCore)
{
    EXPECT_FALSE(foundry::types::SemanticVersion::parse("1.2.3.").has_value());
}

TEST(SemanticVersionTest, RejectsEmptyPrerelease)
{
    EXPECT_FALSE(foundry::types::SemanticVersion::parse("1.2.3-").has_value());
}

TEST(SemanticVersionTest, RejectsTrailingDotInPrerelease)
{
    EXPECT_FALSE(foundry::types::SemanticVersion::parse("1.2.3-alpha.").has_value());
}

TEST(SemanticVersionTest, RejectsEmptyIdentifierInPrerelease)
{
    EXPECT_FALSE(foundry::types::SemanticVersion::parse("1.2.3-alpha..1").has_value());
}

TEST(SemanticVersionTest, RejectsLeadingZeroInNumericPrereleaseIdentifier)
{
    EXPECT_FALSE(foundry::types::SemanticVersion::parse("1.2.3-alpha.01").has_value());
}

TEST(SemanticVersionTest, RejectsLeadingZeroInPureNumericPrerelease)
{
    EXPECT_FALSE(foundry::types::SemanticVersion::parse("1.2.3-01").has_value());
}

TEST(SemanticVersionTest, AcceptsZeroAsNumericPrereleaseIdentifier)
{
    // "0" on its own is valid — only leading zeroes on multi-digit numbers are rejected.
    const auto v = foundry::types::SemanticVersion::parse("1.2.3-0");
    EXPECT_TRUE(v.has_value());
}

TEST(SemanticVersionTest, RejectsEmptyBuildMetadata)
{
    EXPECT_FALSE(foundry::types::SemanticVersion::parse("1.2.3+").has_value());
}

TEST(SemanticVersionTest, RejectsTrailingDotInBuild)
{
    EXPECT_FALSE(foundry::types::SemanticVersion::parse("1.2.3+build.").has_value());
}

TEST(SemanticVersionTest, RejectsEmptyIdentifierInBuild)
{
    EXPECT_FALSE(foundry::types::SemanticVersion::parse("1.2.3+build..42").has_value());
}

// ============================================================================
// Serialization
// ============================================================================

TEST(SemanticVersionTest, CoreStringOmitsPrereleaseAndBuild)
{
    const foundry::types::SemanticVersion v{1, 2, 3, "alpha.1", "build.42"};
    EXPECT_EQ(v.core_string(), "1.2.3");
}

TEST(SemanticVersionTest, ToStringIncludesBuildByDefault)
{
    const foundry::types::SemanticVersion v{1, 2, 3, "alpha.1", "build.42"};
    EXPECT_EQ(v.to_string(), "1.2.3-alpha.1+build.42");
}

TEST(SemanticVersionTest, ToStringExcludesBuildWhenRequested)
{
    const foundry::types::SemanticVersion v{1, 2, 3, "alpha.1", "build.42"};
    EXPECT_EQ(v.to_string(false), "1.2.3-alpha.1");
}

TEST(SemanticVersionTest, ToStringRoundtrips)
{
    const std::string input = "1.2.3-alpha.1+build.42";
    const auto v = foundry::types::SemanticVersion::parse(input);
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->to_string(), input);
}

// ============================================================================
// Comparison — Core Version
// ============================================================================

TEST(SemanticVersionTest, EqualVersionsAreEqual)
{
    const foundry::types::SemanticVersion a{1, 2, 3};
    const foundry::types::SemanticVersion b{1, 2, 3};
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(SemanticVersionTest, MajorVersionDominatesComparison)
{
    EXPECT_LT((foundry::types::SemanticVersion{1, 9, 9}), (foundry::types::SemanticVersion{2, 0, 0}));
}

TEST(SemanticVersionTest, MinorVersionDominatesWhenMajorEqual)
{
    EXPECT_LT((foundry::types::SemanticVersion{1, 1, 9}), (foundry::types::SemanticVersion{1, 2, 0}));
}

TEST(SemanticVersionTest, PatchVersionComparedWhenMajorMinorEqual)
{
    EXPECT_LT((foundry::types::SemanticVersion{1, 2, 3}), (foundry::types::SemanticVersion{1, 2, 4}));
}

TEST(SemanticVersionTest, LargeComponentsCompareCorrectly)
{
    // Regression: ensures no integer encoding is used.
    EXPECT_LT((foundry::types::SemanticVersion{1, 1000, 0}), (foundry::types::SemanticVersion{2, 0, 0}));
    EXPECT_GT((foundry::types::SemanticVersion{1, 1000, 0}), (foundry::types::SemanticVersion{1, 999, 0}));
}

// ============================================================================
// Comparison — Pre-release Precedence
// ============================================================================

// Official precedence sequence from semver.org §11:
//   1.0.0-alpha < 1.0.0-alpha.1 < 1.0.0-alpha.beta
//   < 1.0.0-beta < 1.0.0-beta.2 < 1.0.0-beta.11
//   < 1.0.0-rc.1 < 1.0.0

TEST(SemanticVersionTest, PrereleaseSequenceAlphaLessThanAlpha1)
{
    EXPECT_LT((foundry::types::SemanticVersion{1, 0, 0, "alpha"}),
              (foundry::types::SemanticVersion{1, 0, 0, "alpha.1"}));
}

TEST(SemanticVersionTest, PrereleaseSequenceAlpha1LessThanAlphaBeta)
{
    EXPECT_LT((foundry::types::SemanticVersion{1, 0, 0, "alpha.1"}),
              (foundry::types::SemanticVersion{1, 0, 0, "alpha.beta"}));
}

TEST(SemanticVersionTest, PrereleaseSequenceAlphaBetaLessThanBeta)
{
    EXPECT_LT((foundry::types::SemanticVersion{1, 0, 0, "alpha.beta"}),
              (foundry::types::SemanticVersion{1, 0, 0, "beta"}));
}

TEST(SemanticVersionTest, PrereleaseSequenceBetaLessThanBeta2)
{
    EXPECT_LT((foundry::types::SemanticVersion{1, 0, 0, "beta"}),
              (foundry::types::SemanticVersion{1, 0, 0, "beta.2"}));
}

TEST(SemanticVersionTest, PrereleaseSequenceBeta2LessThanBeta11)
{
    // Numeric identifiers are compared numerically: 2 < 11.
    EXPECT_LT((foundry::types::SemanticVersion{1, 0, 0, "beta.2"}),
              (foundry::types::SemanticVersion{1, 0, 0, "beta.11"}));
}

TEST(SemanticVersionTest, PrereleaseSequenceBeta11LessThanRc1)
{
    EXPECT_LT((foundry::types::SemanticVersion{1, 0, 0, "beta.11"}),
              (foundry::types::SemanticVersion{1, 0, 0, "rc.1"}));
}

TEST(SemanticVersionTest, PrereleaseSequenceRc1LessThanRelease)
{
    // Any pre-release has lower precedence than the associated release.
    EXPECT_LT((foundry::types::SemanticVersion{1, 0, 0, "rc.1"}),
              (foundry::types::SemanticVersion{1, 0, 0}));
}

TEST(SemanticVersionTest, NumericIdentifierLessThanAlphanumeric)
{
    // §11.4.3: numeric < alphanumeric when compared against each other.
    EXPECT_LT((foundry::types::SemanticVersion{1, 0, 0, "1"}),
              (foundry::types::SemanticVersion{1, 0, 0, "alpha"}));
}

TEST(SemanticVersionTest, MoreIdentifiersWinsWhenPrefixEqual)
{
    // §11.4.4: larger field set has higher precedence.
    EXPECT_LT((foundry::types::SemanticVersion{1, 0, 0, "alpha"}),
              (foundry::types::SemanticVersion{1, 0, 0, "alpha.1"}));
}

// ============================================================================
// Comparison — Build Metadata
// ============================================================================

TEST(SemanticVersionTest, BuildMetadataIgnoredInEquality)
{
    const auto v1 = foundry::types::SemanticVersion::parse("1.0.0+build.1");
    const auto v2 = foundry::types::SemanticVersion::parse("1.0.0+build.2");
    ASSERT_TRUE(v1.has_value() && v2.has_value());
    EXPECT_TRUE(*v1 == *v2);
}

TEST(SemanticVersionTest, BuildMetadataIgnoredInLessThan)
{
    const auto v1 = foundry::types::SemanticVersion::parse("1.0.0+build.1");
    const auto v2 = foundry::types::SemanticVersion::parse("1.0.0+build.2");
    ASSERT_TRUE(v1.has_value() && v2.has_value());
    EXPECT_FALSE(*v1 < *v2);
    EXPECT_FALSE(*v2 < *v1);
}

TEST(SemanticVersionTest, BuildMetadataPreservedInToString)
{
    const auto v = foundry::types::SemanticVersion::parse("1.0.0+build.42");
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v->to_string(), "1.0.0+build.42");
}

// ============================================================================
// Version Gate
// ============================================================================

TEST(SemanticVersionTest, AtLeastReturnsTrueForExactMatch)
{
    const foundry::types::SemanticVersion v{2, 3, 1};
    EXPECT_TRUE(v.at_least(2, 3, 1));
}

TEST(SemanticVersionTest, AtLeastReturnsTrueWhenHigher)
{
    const foundry::types::SemanticVersion v{2, 3, 1};
    EXPECT_TRUE(v.at_least(1, 9, 9));
    EXPECT_TRUE(v.at_least(2, 0, 0));
    EXPECT_TRUE(v.at_least(2, 3, 0));
}

TEST(SemanticVersionTest, AtLeastReturnsFalseWhenLower)
{
    const foundry::types::SemanticVersion v{2, 3, 1};
    EXPECT_FALSE(v.at_least(2, 3, 2));
    EXPECT_FALSE(v.at_least(2, 4, 0));
    EXPECT_FALSE(v.at_least(3, 0, 0));
}
