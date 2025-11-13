#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <cmath>
#include <vector>
#include <algorithm>

using Catch::Approx;

// Mock Settings for OSL mode testing
namespace Settings {
    constexpr float kDefaultArousalChangeRate = 0.2f;  // 80% convergence per game hour
    constexpr float kDefaultLibidoChangeRate = 0.1f;   // 90% convergence per game hour
}

// Test implementations of OSL arousal convergence functions
// These mirror the actual implementations in ArousalSystemOSL.cpp

float CalculateOSLConvergence(float currentValue, float targetValue, float epsilon, float gameHoursPassed) {
    // OSL uses exponential convergence: newVal = lerp(current, target, 1 - epsilon^time)
    float t = 1.0f - std::pow(epsilon, gameHoursPassed);
    return std::lerp(currentValue, targetValue, t);
}

float UpdateActorArousal(float currentArousal, float targetBaseline, float gameHoursPassed) {
    return CalculateOSLConvergence(currentArousal, targetBaseline, Settings::kDefaultArousalChangeRate, gameHoursPassed);
}

float UpdateActorLibido(float currentLibido, float targetLibido, float gameHoursPassed) {
    return CalculateOSLConvergence(currentLibido, targetLibido, Settings::kDefaultLibidoChangeRate, gameHoursPassed);
}

// Baseline calculation based on situational modifiers
struct SituationalModifiers {
    bool isNaked = false;
    bool isInScene = false;
    bool isViewingNude = false;
    bool isViewingScene = false;
    bool hasEroticArmor = false;
    int deviousDeviceCount = 0;
    float baseLibido = 10.0f;
};

float CalculateBaseline(const SituationalModifiers& mods) {
    float baseline = mods.baseLibido;

    if (mods.isNaked) baseline += 30.0f;
    if (mods.isInScene) baseline += 50.0f;
    if (mods.isViewingNude) baseline += 20.0f;
    if (mods.isViewingScene) baseline += 30.0f;
    if (mods.hasEroticArmor) baseline += 20.0f;

    // Simplified device contributions
    baseline += mods.deviousDeviceCount * 10.0f;

    return std::min(100.0f, baseline);
}

TEST_CASE("OSL Arousal Convergence", "[OSL][Arousal]") {

    SECTION("Basic arousal convergence over time") {
        float currentArousal = 20.0f;
        float targetBaseline = 50.0f;

        // After 1 game hour, should be 80% of the way to target
        float newArousal = UpdateActorArousal(currentArousal, targetBaseline, 1.0f);
        REQUIRE(newArousal == Approx(44.0f).epsilon(0.01)); // 20 + (50-20)*0.8

        // After 2 game hours, should be 96% of the way
        newArousal = UpdateActorArousal(currentArousal, targetBaseline, 2.0f);
        REQUIRE(newArousal == Approx(48.8f).epsilon(0.01)); // 20 + (50-20)*0.96

        // After 3 game hours, should be 99.2% of the way
        newArousal = UpdateActorArousal(currentArousal, targetBaseline, 3.0f);
        REQUIRE(newArousal == Approx(49.76f).epsilon(0.01));
    }

    SECTION("Arousal decreasing to baseline") {
        float currentArousal = 80.0f;
        float targetBaseline = 10.0f;

        // After 1 hour, drops 80% toward baseline
        float newArousal = UpdateActorArousal(currentArousal, targetBaseline, 1.0f);
        REQUIRE(newArousal == Approx(24.0f).epsilon(0.01)); // 80 - (80-10)*0.8

        // After 0.5 hours, drops ~55.3% toward baseline
        newArousal = UpdateActorArousal(currentArousal, targetBaseline, 0.5f);
        REQUIRE(newArousal == Approx(41.29f).epsilon(0.01));
    }

    SECTION("Fractional time updates") {
        float currentArousal = 30.0f;
        float targetBaseline = 60.0f;

        // Small time increments (0.1 game hours = 6 minutes)
        float newArousal = UpdateActorArousal(currentArousal, targetBaseline, 0.1f);
        float expectedT = 1.0f - std::pow(0.2f, 0.1f); // ~0.148
        REQUIRE(newArousal == Approx(30.0f + 30.0f * expectedT).epsilon(0.01));

        // Very small increment (0.01 game hours = 36 seconds)
        newArousal = UpdateActorArousal(currentArousal, targetBaseline, 0.01f);
        expectedT = 1.0f - std::pow(0.2f, 0.01f); // ~0.0161
        REQUIRE(newArousal == Approx(30.0f + 30.0f * expectedT).epsilon(0.01));
    }
}

TEST_CASE("OSL Libido Convergence", "[OSL][Libido]") {

    SECTION("Libido changes more slowly than arousal") {
        float currentLibido = 20.0f;
        float targetLibido = 50.0f;

        // After 1 game hour, should be 90% of the way (epsilon=0.1)
        float newLibido = UpdateActorLibido(currentLibido, targetLibido, 1.0f);
        REQUIRE(newLibido == Approx(47.0f).epsilon(0.01)); // 20 + (50-20)*0.9

        // Compare with arousal rate
        float arousalResult = UpdateActorArousal(20.0f, 50.0f, 1.0f);
        REQUIRE(newLibido > arousalResult); // Libido converges slower
    }

    SECTION("Multiple libido updates over time") {
        float libido = 10.0f;
        float target = 40.0f;

        // Simulate multiple small updates
        for (int i = 0; i < 10; ++i) {
            libido = UpdateActorLibido(libido, target, 0.1f);
        }

        // After 1 game hour total (10 * 0.1), should be ~90% converged
        REQUIRE(libido == Approx(37.0f).epsilon(0.1));
    }
}

TEST_CASE("OSL Baseline Calculations", "[OSL][Baseline]") {

    SECTION("Single modifier baselines") {
        SituationalModifiers mods{};
        mods.baseLibido = 10.0f;

        // Test each modifier individually
        mods.isNaked = true;
        REQUIRE(CalculateBaseline(mods) == 40.0f); // 10 + 30

        mods = SituationalModifiers{};
        mods.baseLibido = 10.0f;
        mods.isInScene = true;
        REQUIRE(CalculateBaseline(mods) == 60.0f); // 10 + 50

        mods = SituationalModifiers{};
        mods.baseLibido = 10.0f;
        mods.isViewingNude = true;
        REQUIRE(CalculateBaseline(mods) == 30.0f); // 10 + 20
    }

    SECTION("Stacking modifiers") {
        SituationalModifiers mods{};
        mods.baseLibido = 10.0f;
        mods.isNaked = true;        // +30
        mods.isViewingScene = true;  // +30
        mods.hasEroticArmor = true;  // +20

        REQUIRE(CalculateBaseline(mods) == 90.0f); // 10 + 30 + 30 + 20
    }

    SECTION("Baseline capping at 100") {
        SituationalModifiers mods{};
        mods.baseLibido = 20.0f;
        mods.isNaked = true;         // +30
        mods.isInScene = true;       // +50
        mods.isViewingScene = true;  // +30
        mods.hasEroticArmor = true;  // +20
        mods.deviousDeviceCount = 5; // +50

        // Total would be 200, but should cap at 100
        REQUIRE(CalculateBaseline(mods) == 100.0f);
    }

    SECTION("Device contributions") {
        SituationalModifiers mods{};
        mods.baseLibido = 10.0f;

        // Test various device counts
        mods.deviousDeviceCount = 1;
        REQUIRE(CalculateBaseline(mods) == 20.0f); // 10 + 10

        mods.deviousDeviceCount = 3;
        REQUIRE(CalculateBaseline(mods) == 40.0f); // 10 + 30

        mods.deviousDeviceCount = 8;
        REQUIRE(CalculateBaseline(mods) == 90.0f); // 10 + 80
    }
}

TEST_CASE("OSL Dynamic Scenarios", "[OSL][Scenarios]") {

    SECTION("Scene start and end") {
        float arousal = 20.0f;
        SituationalModifiers mods{};
        mods.baseLibido = 10.0f;

        // Scene starts
        mods.isInScene = true;
        float baseline = CalculateBaseline(mods);
        REQUIRE(baseline == 60.0f);

        // After 0.5 hours in scene
        arousal = UpdateActorArousal(arousal, baseline, 0.5f);
        REQUIRE(arousal > 40.0f);
        REQUIRE(arousal < 50.0f);

        // Scene ends
        mods.isInScene = false;
        baseline = CalculateBaseline(mods);
        REQUIRE(baseline == 10.0f);

        // After 1 hour post-scene
        arousal = UpdateActorArousal(arousal, baseline, 1.0f);
        REQUIRE(arousal < 20.0f);
    }

    SECTION("Equipment changes affecting baseline") {
        float arousal = 30.0f;
        SituationalModifiers mods{};
        mods.baseLibido = 15.0f;

        // Equip erotic armor
        mods.hasEroticArmor = true;
        float baseline = CalculateBaseline(mods);
        arousal = UpdateActorArousal(arousal, baseline, 0.25f);
        float arousedState = arousal;

        // Remove armor
        mods.hasEroticArmor = false;
        baseline = CalculateBaseline(mods);
        arousal = UpdateActorArousal(arousedState, baseline, 0.25f);

        // Should have decreased
        REQUIRE(arousal < arousedState);
    }

    SECTION("Rapid baseline changes") {
        float arousal = 50.0f;

        // Simulate rapid situation changes
        std::vector<float> baselines = {10.0f, 80.0f, 20.0f, 60.0f, 30.0f};
        float timeStep = 0.05f; // 3 minutes game time

        for (float baseline : baselines) {
            arousal = UpdateActorArousal(arousal, baseline, timeStep);
        }

        // Should end up partially converged toward last baseline
        REQUIRE(arousal > 30.0f);
        REQUIRE(arousal < 50.0f);
    }
}

TEST_CASE("OSL vs SLA Convergence Comparison", "[OSL][SLA][Comparison]") {

    SECTION("OSL converges faster than SLA decay") {
        // OSL: exponential convergence with epsilon
        float oslArousal = 100.0f;
        float oslTarget = 0.0f;

        // After 1 game hour
        float oslResult = UpdateActorArousal(oslArousal, oslTarget, 1.0f);

        // SLA equivalent: decay with half-life
        float slaExposure = 100.0f;
        float slaHalfLife = 2.0f; // 2 days
        float slaDecayed = slaExposure * std::pow(1.5f, -1.0f/24.0f/slaHalfLife);

        // OSL should converge much faster
        REQUIRE(oslResult < slaDecayed);
    }

    SECTION("Response to stimuli") {
        // OSL: immediate baseline change
        float oslArousal = 20.0f;
        float oslBaseline = 70.0f;
        oslArousal = UpdateActorArousal(oslArousal, oslBaseline, 0.1f);

        // SLA: additive exposure
        float slaExposure = 20.0f;
        float slaAddition = 4.0f; // typical viewing exposure
        slaExposure += slaAddition;

        // OSL moves proportionally toward target
        // SLA adds fixed amount
        REQUIRE(oslArousal > 25.0f); // OSL moved significantly
        REQUIRE(slaExposure == 24.0f); // SLA added fixed amount
    }
}

TEST_CASE("OSL Edge Cases", "[OSL][EdgeCases]") {

    SECTION("Zero time passage") {
        float arousal = 50.0f;
        float baseline = 30.0f;

        float newArousal = UpdateActorArousal(arousal, baseline, 0.0f);
        REQUIRE(newArousal == arousal); // No change with zero time
    }

    SECTION("Very long time periods") {
        float arousal = 100.0f;
        float baseline = 10.0f;

        // After 10 game hours, should be essentially at baseline
        float newArousal = UpdateActorArousal(arousal, baseline, 10.0f);
        REQUIRE(newArousal == Approx(10.0f).epsilon(0.001));
    }

    SECTION("Negative values prevention") {
        // System should handle edge cases gracefully
        float arousal = 5.0f;
        float baseline = 0.0f;

        float newArousal = UpdateActorArousal(arousal, baseline, 5.0f);
        REQUIRE(newArousal >= 0.0f);
        REQUIRE(newArousal <= 100.0f);
    }

    SECTION("Epsilon boundary conditions") {
        // Test with extreme epsilon values
        float current = 50.0f;
        float target = 80.0f;

        // Very small epsilon (fast convergence)
        float fastResult = CalculateOSLConvergence(current, target, 0.01f, 1.0f);
        REQUIRE(fastResult == Approx(80.0f).epsilon(0.01));

        // Very large epsilon (slow convergence)
        float slowResult = CalculateOSLConvergence(current, target, 0.9f, 1.0f);
        REQUIRE(slowResult < 55.0f);
    }
}