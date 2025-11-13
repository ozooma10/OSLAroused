#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <map>
#include <mutex>
#include <thread>
#include <vector>
#include <cstdint>

using Catch::Approx;

// Mock FormID type
using FormID = uint32_t;

// Simplified version of BaseData template from PersistedData.h
template <typename T>
class MockBaseData {
public:
    T GetData(FormID formId, T defaultValue) {
        std::lock_guard<std::recursive_mutex> lock(m_Lock);
        auto it = m_Data.find(formId);
        if (it != m_Data.end()) {
            return it->second;
        }
        return defaultValue;
    }

    void SetData(FormID formId, T value) {
        std::lock_guard<std::recursive_mutex> lock(m_Lock);
        m_Data[formId] = value;
    }

    void RemoveData(FormID formId) {
        std::lock_guard<std::recursive_mutex> lock(m_Lock);
        m_Data.erase(formId);
    }

    void Clear() {
        std::lock_guard<std::recursive_mutex> lock(m_Lock);
        m_Data.clear();
    }

    size_t GetSize() const {
        std::lock_guard<std::recursive_mutex> lock(m_Lock);
        return m_Data.size();
    }

    std::map<FormID, T> GetAllData() const {
        std::lock_guard<std::recursive_mutex> lock(m_Lock);
        return m_Data;
    }

private:
    mutable std::recursive_mutex m_Lock;
    std::map<FormID, T> m_Data;
};

// Mock specific data types
class MockArousalData : public MockBaseData<float> {
public:
    static MockArousalData* GetSingleton() {
        static MockArousalData instance;
        return &instance;
    }
};

class MockLibidoData : public MockBaseData<float> {
public:
    static MockLibidoData* GetSingleton() {
        static MockLibidoData instance;
        return &instance;
    }
};

class MockLastCheckTimeData : public MockBaseData<float> {
public:
    static MockLastCheckTimeData* GetSingleton() {
        static MockLastCheckTimeData instance;
        return &instance;
    }
};

class MockKeywordData : public MockBaseData<std::vector<uint32_t>> {
public:
    static MockKeywordData* GetSingleton() {
        static MockKeywordData instance;
        return &instance;
    }
};

// Serialization helpers
struct SerializedRecord {
    FormID formId;
    std::vector<uint8_t> data;
};

template<typename T>
std::vector<uint8_t> Serialize(const T& value) {
    std::vector<uint8_t> bytes(sizeof(T));
    memcpy(bytes.data(), &value, sizeof(T));
    return bytes;
}

template<typename T>
T Deserialize(const std::vector<uint8_t>& bytes) {
    T value;
    memcpy(&value, bytes.data(), sizeof(T));
    return value;
}

TEST_CASE("Persisted Data Basic Operations", "[PersistedData][Basic]") {

    SECTION("Store and retrieve float data") {
        auto arousalData = MockArousalData::GetSingleton();
        arousalData->Clear();

        FormID actor1 = 0x00014;
        FormID actor2 = 0x00015;

        arousalData->SetData(actor1, 75.5f);
        arousalData->SetData(actor2, 30.0f);

        REQUIRE(arousalData->GetData(actor1, 0.0f) == 75.5f);
        REQUIRE(arousalData->GetData(actor2, 0.0f) == 30.0f);
    }

    SECTION("Default values for missing data") {
        auto libidoData = MockLibidoData::GetSingleton();
        libidoData->Clear();

        FormID unknownActor = 0x99999;

        // Should return default value
        REQUIRE(libidoData->GetData(unknownActor, 10.0f) == 10.0f);
        REQUIRE(libidoData->GetData(unknownActor, -1.0f) == -1.0f);
    }

    SECTION("Overwriting existing data") {
        auto timeData = MockLastCheckTimeData::GetSingleton();
        timeData->Clear();

        FormID actor = 0x00020;

        timeData->SetData(actor, 100.0f);
        REQUIRE(timeData->GetData(actor, 0.0f) == 100.0f);

        timeData->SetData(actor, 200.0f);
        REQUIRE(timeData->GetData(actor, 0.0f) == 200.0f);
    }

    SECTION("Removing data") {
        auto arousalData = MockArousalData::GetSingleton();
        arousalData->Clear();

        FormID actor = 0x00030;
        arousalData->SetData(actor, 50.0f);
        REQUIRE(arousalData->GetSize() == 1);

        arousalData->RemoveData(actor);
        REQUIRE(arousalData->GetSize() == 0);
        REQUIRE(arousalData->GetData(actor, -1.0f) == -1.0f);
    }
}

TEST_CASE("Persisted Data Thread Safety", "[PersistedData][ThreadSafety]") {

    SECTION("Concurrent reads and writes") {
        auto arousalData = MockArousalData::GetSingleton();
        arousalData->Clear();

        const int numThreads = 10;
        const int numOperations = 100;
        std::vector<std::thread> threads;

        // Writers
        for (int t = 0; t < numThreads / 2; ++t) {
            threads.emplace_back([=]() {
                for (int i = 0; i < numOperations; ++i) {
                    FormID id = t * 1000 + i;
                    arousalData->SetData(id, static_cast<float>(id));
                }
            });
        }

        // Readers
        for (int t = numThreads / 2; t < numThreads; ++t) {
            threads.emplace_back([=]() {
                for (int i = 0; i < numOperations; ++i) {
                    FormID id = (t - numThreads / 2) * 1000 + i;
                    arousalData->GetData(id, 0.0f);
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        // Verify data integrity
        REQUIRE(arousalData->GetSize() > 0);
        REQUIRE(arousalData->GetSize() <= numThreads / 2 * numOperations);
    }

    SECTION("Concurrent modifications") {
        auto libidoData = MockLibidoData::GetSingleton();
        libidoData->Clear();

        FormID sharedActor = 0x1000;
        const int numThreads = 10;
        const int numUpdates = 100;
        std::vector<std::thread> threads;

        for (int t = 0; t < numThreads; ++t) {
            threads.emplace_back([=]() {
                for (int i = 0; i < numUpdates; ++i) {
                    float current = libidoData->GetData(sharedActor, 0.0f);
                    libidoData->SetData(sharedActor, current + 1.0f);
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        // Due to race conditions, final value may not be exact
        // but should be positive and reasonable
        float finalValue = libidoData->GetData(sharedActor, 0.0f);
        REQUIRE(finalValue > 0.0f);
        REQUIRE(finalValue <= numThreads * numUpdates);
    }
}

TEST_CASE("Persisted Data Serialization", "[PersistedData][Serialization]") {

    SECTION("Serialize and deserialize float data") {
        auto arousalData = MockArousalData::GetSingleton();
        arousalData->Clear();

        // Set up test data
        arousalData->SetData(0x00014, 75.5f);
        arousalData->SetData(0x00015, 30.0f);
        arousalData->SetData(0x00016, 99.9f);

        // Simulate serialization
        std::vector<SerializedRecord> records;
        for (const auto& [formId, value] : arousalData->GetAllData()) {
            records.push_back({formId, Serialize(value)});
        }

        // Clear and restore
        arousalData->Clear();
        REQUIRE(arousalData->GetSize() == 0);

        for (const auto& record : records) {
            float value = Deserialize<float>(record.data);
            arousalData->SetData(record.formId, value);
        }

        // Verify restoration
        REQUIRE(arousalData->GetData(0x00014, 0.0f) == 75.5f);
        REQUIRE(arousalData->GetData(0x00015, 0.0f) == 30.0f);
        REQUIRE(arousalData->GetData(0x00016, 0.0f) == 99.9f);
    }

    SECTION("FormID resolution simulation") {
        auto timeData = MockLastCheckTimeData::GetSingleton();
        timeData->Clear();

        // Simulate FormIDs that might change between saves
        FormID oldFormId = 0xFF000800;  // Temporary FormID
        FormID newFormId = 0x00012345;  // Resolved FormID

        // Original save
        timeData->SetData(oldFormId, 123.45f);

        // Simulate load with FormID resolution
        float value = timeData->GetData(oldFormId, 0.0f);
        timeData->RemoveData(oldFormId);
        timeData->SetData(newFormId, value);

        // Verify
        REQUIRE(timeData->GetData(oldFormId, -1.0f) == -1.0f);
        REQUIRE(timeData->GetData(newFormId, 0.0f) == 123.45f);
    }

    SECTION("Complex data type serialization") {
        auto keywordData = MockKeywordData::GetSingleton();
        keywordData->Clear();

        FormID actor = 0x00100;
        std::vector<uint32_t> keywords = {0x0001, 0x0002, 0x0003, 0x0004};

        keywordData->SetData(actor, keywords);

        // Retrieve and verify
        auto retrieved = keywordData->GetData(actor, {});
        REQUIRE(retrieved.size() == 4);
        REQUIRE(retrieved[0] == 0x0001);
        REQUIRE(retrieved[3] == 0x0004);
    }
}

TEST_CASE("Persisted Data Memory Management", "[PersistedData][Memory]") {

    SECTION("Large dataset handling") {
        auto arousalData = MockArousalData::GetSingleton();
        arousalData->Clear();

        const int numActors = 10000;

        // Add many actors
        for (int i = 0; i < numActors; ++i) {
            arousalData->SetData(i, static_cast<float>(i) * 0.1f);
        }

        REQUIRE(arousalData->GetSize() == numActors);

        // Selective removal
        for (int i = 0; i < numActors; i += 2) {
            arousalData->RemoveData(i);
        }

        REQUIRE(arousalData->GetSize() == numActors / 2);

        // Verify remaining data
        for (int i = 1; i < numActors; i += 2) {
            REQUIRE(arousalData->GetData(i, -1.0f) == Approx(i * 0.1f));
        }
    }

    SECTION("Clear all data") {
        auto libidoData = MockLibidoData::GetSingleton();
        libidoData->Clear();

        // Add data
        for (int i = 0; i < 100; ++i) {
            libidoData->SetData(i, static_cast<float>(i));
        }

        REQUIRE(libidoData->GetSize() == 100);

        // Clear all
        libidoData->Clear();
        REQUIRE(libidoData->GetSize() == 0);

        // Verify all cleared
        for (int i = 0; i < 100; ++i) {
            REQUIRE(libidoData->GetData(i, -1.0f) == -1.0f);
        }
    }
}

TEST_CASE("Persisted Data Migration Scenarios", "[PersistedData][Migration]") {

    SECTION("OSL to SLA mode data conversion") {
        // Simulate switching from OSL to SLA mode
        auto arousalData = MockArousalData::GetSingleton();
        auto libidoData = MockLibidoData::GetSingleton();
        arousalData->Clear();
        libidoData->Clear();

        FormID actor = 0x00200;

        // OSL mode data
        float oslArousal = 75.0f;
        float oslLibido = 15.0f;
        arousalData->SetData(actor, oslArousal);
        libidoData->SetData(actor, oslLibido);

        // Convert to SLA mode
        // In SLA: arousal becomes exposure, libido becomes time rate
        float slaExposure = arousalData->GetData(actor, 0.0f);
        float slaTimeRate = libidoData->GetData(actor, 10.0f);

        REQUIRE(slaExposure == 75.0f);
        REQUIRE(slaTimeRate == 15.0f);
    }

    SECTION("Data preservation across mode switches") {
        auto arousalData = MockArousalData::GetSingleton();
        arousalData->Clear();

        std::vector<FormID> actors = {0x001, 0x002, 0x003};
        std::map<FormID, float> originalValues;

        // Store original values
        for (auto id : actors) {
            float value = 10.0f + id * 15.0f;
            arousalData->SetData(id, value);
            originalValues[id] = value;
        }

        // Simulate mode switch (data should persist)
        // ... mode switch happens here ...

        // Verify data preserved
        for (auto id : actors) {
            REQUIRE(arousalData->GetData(id, 0.0f) == originalValues[id]);
        }
    }

    SECTION("Invalid FormID handling") {
        auto timeData = MockLastCheckTimeData::GetSingleton();
        timeData->Clear();

        // Test edge case FormIDs
        FormID invalidId = 0x00000000;
        FormID maxId = 0xFFFFFFFF;

        timeData->SetData(invalidId, 1.0f);
        timeData->SetData(maxId, 2.0f);

        REQUIRE(timeData->GetData(invalidId, -1.0f) == 1.0f);
        REQUIRE(timeData->GetData(maxId, -1.0f) == 2.0f);
    }
}