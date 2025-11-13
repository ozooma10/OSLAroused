#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <cmath>
#include <iostream>

using Catch::Approx;

// Mock the Settings class for testing
namespace Settings {
    constexpr float kSLADecayBase = 1.5f;  // Default decay base from the actual code
}

// Test implementations of the exposure calculation functions
// These mirror the actual implementations in ArousalSystemSLA.cpp

float CalculateExposureDecay(float currentExposure, float timeSinceUpdate, float halfLife) {
    if (halfLife <= 0.1f) {
        return currentExposure;
    }
    // This is the formula from ArousalSystemSLA::GetExposure and ModifyArousal
    return currentExposure * std::pow(Settings::kSLADecayBase, -timeSinceUpdate / halfLife);
}

float CalculateExposureWithModification(float currentExposure, float modification, float timeSinceUpdate, float halfLife) {
    float decayedExposure = CalculateExposureDecay(currentExposure, timeSinceUpdate, halfLife);
    return decayedExposure + modification;
}

float CalculateTimeRateDecay(float baseTimeRate, float daysSinceOrgasm, float halfLife) {
    if (halfLife <= 0.1f) {
        return baseTimeRate;
    }
    // This is the formula from ArousalSystemSLA::GetLibido
    return baseTimeRate * std::pow(Settings::kSLADecayBase, -daysSinceOrgasm / halfLife);
}

float CalculateSpectatingExposureScale(float elapsedGameTime, float updateInterval = 0.1f) {
    // From ArousalSystemSLA::HandleSpectatingNaked
    // Scale reaches 1.0 at the configured update interval
    return std::min(1.0f, elapsedGameTime / updateInterval);
}

float CalculateSLAArousal(float exposure, float daysSinceOrgasm, float timeRate) {
    // From ArousalSystemSLA::GetArousal
    return (daysSinceOrgasm * timeRate) + exposure;
}

TEST_CASE("SLA Exposure Decay Calculations", "[SLA][Exposure]") {

    SECTION("Basic exposure decay with 2-day half-life") {
        float exposure = 100.0f;
        float halfLife = 2.0f; // 2 game days

        // After 2 days (1 half-life), exposure decays by factor of 1/1.5 = ~66.67%
        float decayed = CalculateExposureDecay(exposure, 2.0f, halfLife);
        REQUIRE(decayed == Approx(66.67f).epsilon(0.01));

        // After 4 days (2 half-lives), exposure decays by factor of 1/1.5^2 = ~44.44%
        decayed = CalculateExposureDecay(exposure, 4.0f, halfLife);
        REQUIRE(decayed == Approx(44.44f).epsilon(0.01));

        // After 1 day (0.5 half-lives), exposure decays by factor of 1/1.5^0.5 = ~81.65%
        decayed = CalculateExposureDecay(exposure, 1.0f, halfLife);
        REQUIRE(decayed == Approx(81.65f).epsilon(0.01));
    }

    SECTION("No decay when halfLife is too small") {
        float exposure = 100.0f;
        float halfLife = 0.05f; // Below threshold

        float decayed = CalculateExposureDecay(exposure, 10.0f, halfLife);
        REQUIRE(decayed == exposure);
    }

    SECTION("Exposure decay over multiple time periods") {
        float exposure = 80.0f;
        float halfLife = 2.0f;

        // Test decay over fractional days
        // After 0.5 days: 80 * 1.5^(-0.5/2) = 80 * 1.5^(-0.25) = ~72.29
        float decayed = CalculateExposureDecay(exposure, 0.5f, halfLife);
        REQUIRE(decayed == Approx(72.29f).epsilon(0.01));

        // After 0.25 days: 80 * 1.5^(-0.25/2) = 80 * 1.5^(-0.125) = ~76.21
        decayed = CalculateExposureDecay(exposure, 0.25f, halfLife);
        REQUIRE(decayed == Approx(76.21f).epsilon(0.01));
    }
}

TEST_CASE("SLA Exposure Modification with Decay", "[SLA][Exposure]") {

    SECTION("Adding exposure with time-based decay") {
        float currentExposure = 50.0f;
        float modification = 20.0f;
        float timeSinceUpdate = 1.0f; // 1 game day
        float halfLife = 2.0f;

        float newExposure = CalculateExposureWithModification(currentExposure, modification, timeSinceUpdate, halfLife);

        // First decay: 50 * 1.5^(-1/2) = 50 * 1.5^(-0.5) = ~40.82
        // Then add: 40.82 + 20 = ~60.82
        REQUIRE(newExposure == Approx(60.82f).epsilon(0.01));
    }

    SECTION("Multiple modifications over time") {
        float exposure = 30.0f;
        float halfLife = 2.0f;

        // First modification after 0.5 days
        // Decay: 30 * 1.5^(-0.5/2) = ~27.11
        // Add: 27.11 + 10 = ~37.11
        exposure = CalculateExposureWithModification(exposure, 10.0f, 0.5f, halfLife);
        REQUIRE(exposure == Approx(37.11f).epsilon(0.01));

        // Second modification after another 0.5 days
        // Decay: 37.11 * 1.5^(-0.5/2) = ~33.53
        // Add: 33.53 + 5 = ~38.53
        exposure = CalculateExposureWithModification(exposure, 5.0f, 0.5f, halfLife);
        REQUIRE(exposure == Approx(38.53f).epsilon(0.01));
    }
}

TEST_CASE("SLA Time Rate Decay", "[SLA][TimeRate]") {

    SECTION("Time rate decay based on days since orgasm") {
        float baseTimeRate = 10.0f;
        float halfLife = 2.0f;

        // After 2 days, time rate decays by factor of 1/1.5 = ~66.67%
        float timeRate = CalculateTimeRateDecay(baseTimeRate, 2.0f, halfLife);
        REQUIRE(timeRate == Approx(6.667f).epsilon(0.01));

        // After 1 day, time rate decays by factor of 1/1.5^0.5 = ~81.65%
        timeRate = CalculateTimeRateDecay(baseTimeRate, 1.0f, halfLife);
        REQUIRE(timeRate == Approx(8.165f).epsilon(0.01));

        // After 4 days, time rate decays by factor of 1/1.5^2 = ~44.44%
        timeRate = CalculateTimeRateDecay(baseTimeRate, 4.0f, halfLife);
        REQUIRE(timeRate == Approx(4.444f).epsilon(0.01));
    }

    SECTION("Time rate with no decay") {
        float baseTimeRate = 10.0f;
        float halfLife = 0.05f; // Below threshold

        float timeRate = CalculateTimeRateDecay(baseTimeRate, 10.0f, halfLife);
        REQUIRE(timeRate == baseTimeRate);
    }
}

TEST_CASE("SLA Spectating Exposure Scaling", "[SLA][Spectating]") {

    SECTION("Exposure scaling based on update interval") {
        float updateInterval = 0.1f; // Default 0.1 game hours

        // At exactly the update interval, scale should be 1.0
        float scale = CalculateSpectatingExposureScale(0.1f, updateInterval);
        REQUIRE(scale == 1.0f);

        // At half the update interval, scale should be 0.5
        scale = CalculateSpectatingExposureScale(0.05f, updateInterval);
        REQUIRE(scale == 0.5f);

        // Beyond update interval, scale should cap at 1.0
        scale = CalculateSpectatingExposureScale(0.2f, updateInterval);
        REQUIRE(scale == 1.0f);
    }

    SECTION("Spectating exposure amounts") {
        float baseExposurePreferred = 4.0f;
        float baseExposureNonPreferred = 2.0f;
        float exhibitionistBonus = 2.0f;

        // Test with different time scales
        float elapsed = 0.05f; // Half update interval
        float updateInterval = 0.1f;
        float scale = CalculateSpectatingExposureScale(elapsed, updateInterval);

        REQUIRE(scale == 0.5f);

        // Preferred gender exposure
        float preferredExposure = baseExposurePreferred * scale;
        REQUIRE(preferredExposure == 2.0f);

        // Non-preferred gender exposure
        float nonPreferredExposure = baseExposureNonPreferred * scale;
        REQUIRE(nonPreferredExposure == 1.0f);

        // Exhibitionist bonus
        float exhibExposure = exhibitionistBonus * scale;
        REQUIRE(exhibExposure == 1.0f);
    }

    SECTION("Spectating with varying update intervals") {
        // Test that exposure scaling works correctly with different update intervals
        float elapsed = 0.05f;

        // Standard interval (0.1 hours)
        float scale1 = CalculateSpectatingExposureScale(elapsed, 0.1f);
        REQUIRE(scale1 == 0.5f);

        // Faster interval (0.05 hours)
        float scale2 = CalculateSpectatingExposureScale(elapsed, 0.05f);
        REQUIRE(scale2 == 1.0f);

        // Slower interval (0.2 hours)
        float scale3 = CalculateSpectatingExposureScale(elapsed, 0.2f);
        REQUIRE(scale3 == 0.25f);
    }
}

TEST_CASE("SLA Total Arousal Calculation", "[SLA][Arousal]") {

    SECTION("Basic arousal calculation") {
        float exposure = 30.0f;
        float daysSinceOrgasm = 2.0f;
        float timeRate = 10.0f;

        float arousal = CalculateSLAArousal(exposure, daysSinceOrgasm, timeRate);
        REQUIRE(arousal == 50.0f); // (2 * 10) + 30
    }

    SECTION("Arousal with decayed values") {
        float baseExposure = 50.0f;
        float halfLife = 2.0f;
        float timeSinceUpdate = 1.0f;

        // Decay exposure: 50 * 1.5^(-1/2) = ~40.82
        float decayedExposure = CalculateExposureDecay(baseExposure, timeSinceUpdate, halfLife);

        float daysSinceOrgasm = 3.0f;
        float baseTimeRate = 10.0f;

        // Decay time rate: 10 * 1.5^(-3/2) = ~5.44
        float decayedTimeRate = CalculateTimeRateDecay(baseTimeRate, daysSinceOrgasm, halfLife);

        // Calculate final arousal
        float arousal = CalculateSLAArousal(decayedExposure, daysSinceOrgasm, decayedTimeRate);

        // Expected: exposure ~40.82, time rate ~5.44, arousal = (3 * 5.44) + 40.82 = ~57.15
        REQUIRE(arousal == Approx(57.15f).epsilon(0.1));
    }
}

TEST_CASE("SLA Edge Cases and Boundaries", "[SLA][EdgeCases]") {

    SECTION("Zero exposure decay") {
        float exposure = 0.0f;
        float decayed = CalculateExposureDecay(exposure, 10.0f, 2.0f);
        REQUIRE(decayed == 0.0f);
    }

    SECTION("Very long time periods") {
        float exposure = 100.0f;
        float halfLife = 2.0f;

        // After 10 days (5 half-lives): 100 * 1.5^(-10/2) = 100 * 1.5^(-5) = ~13.17%
        float decayed = CalculateExposureDecay(exposure, 10.0f, halfLife);
        REQUIRE(decayed == Approx(13.17f).epsilon(0.01));

        // After 20 days (10 half-lives): 100 * 1.5^(-20/2) = 100 * 1.5^(-10) = ~1.73%
        decayed = CalculateExposureDecay(exposure, 20.0f, halfLife);
        REQUIRE(decayed < 2.0f);
    }

    SECTION("Very short time periods") {
        float exposure = 100.0f;
        float halfLife = 2.0f;

        // After 0.001 days: 100 * 1.5^(-0.001/2) = ~99.97%
        float decayed = CalculateExposureDecay(exposure, 0.001f, halfLife);
        REQUIRE(decayed == Approx(99.97f).epsilon(0.01));
    }

    SECTION("Clamping behavior simulation") {
        // Simulate how the actual game would clamp values
        auto clamp = [](float value, float min, float max) {
            return std::min(std::max(value, min), max);
        };

        float exposure = 120.0f; // Over 100
        exposure = clamp(exposure, 0.0f, 100.0f);
        REQUIRE(exposure == 100.0f);

        exposure = -10.0f; // Below 0
        exposure = clamp(exposure, 0.0f, 100.0f);
        REQUIRE(exposure == 0.0f);
    }
}

TEST_CASE("SLA Real-World Scenarios", "[SLA][Scenarios]") {

    SECTION("Player views nude NPC over time") {
        float exposure = 10.0f;
        float halfLife = 2.0f;
        float updateInterval = 0.1f; // 6 minutes real-time at 20:1 timescale

        // Simulate 5 updates over 0.5 game hours
        for (int i = 0; i < 5; ++i) {
            // Each update adds 4 exposure (preferred gender)
            exposure = CalculateExposureWithModification(exposure, 4.0f, updateInterval, halfLife);
        }

        // Should accumulate exposure with minimal decay between updates
        // With base 1.5, decay per update is minimal
        REQUIRE(exposure > 28.0f);
        REQUIRE(exposure < 32.0f);
    }

    SECTION("Long play session without stimuli") {
        float exposure = 80.0f;
        float halfLife = 2.0f;

        // Simulate 6 game days passing
        // 80 * 1.5^(-6/2) = 80 * 1.5^(-3) = ~23.7
        float decayed = CalculateExposureDecay(exposure, 6.0f, halfLife);

        REQUIRE(decayed == Approx(23.7f).epsilon(0.5));
    }

    SECTION("Orgasm event impact on arousal") {
        float exposure = 40.0f;
        float baseTimeRate = 10.0f;
        float halfLife = 2.0f;

        // Before orgasm: 5 days since last
        float timeRate = CalculateTimeRateDecay(baseTimeRate, 5.0f, halfLife);
        float arousalBefore = CalculateSLAArousal(exposure, 5.0f, timeRate);

        // After orgasm: 0 days since last
        timeRate = CalculateTimeRateDecay(baseTimeRate, 0.0f, halfLife);
        float arousalAfter = CalculateSLAArousal(exposure, 0.0f, timeRate);

        // Arousal should drop significantly after orgasm
        REQUIRE(arousalBefore > arousalAfter);
        REQUIRE(arousalAfter == 40.0f); // Just the exposure component
    }
}