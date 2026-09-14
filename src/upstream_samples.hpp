#pragma once
// Adapted from TwilitRealm/dusklight src/dusk/interp/samples.h at d34226ad1.
// The sampling algorithm is unchanged. Clock and Lerp bind upstream behavior
// through the mod interface instead of linking unexported host implementation.
#include <cstddef>
#include <cstdint>
#include <vector>
namespace gz::upstream {
template <typename T, typename Clock, typename Lerp>
class Samples {
public:
    void reset() { m_samples.clear(); }

    template <typename Sample>
    void capture(int count, Sample sample) {
        if (!Clock::should_capture()) {
            return;
        }
        if (count <= 0) {
            reset();
            return;
        }

        const uint64_t tick = Clock::sim_tick_seq();
        const uint64_t epoch = Clock::presentation_epoch();
        const bool continuous = epoch == m_epoch && (tick == m_tick || tick == m_tick + 1);
        if (!continuous) {
            reset();
        }
        if (count < m_samples.size()) {
            m_samples.erase(m_samples.begin() + count, m_samples.end());
        }
        m_samples.reserve(count);
        for (int i = 0; i < count; ++i) {
            T current = sample(i);
            if (i == m_samples.size()) {
                m_samples.push_back({current, current});
                continue;
            }
            auto& entry = m_samples[i];
            if (tick != m_tick) {
                entry.previous = entry.current;
            }
            entry.current = current;
        }
        m_tick = tick;
        m_epoch = epoch;
    }

    void capture(const T* source, int count) {
        capture(source != nullptr ? count : 0, [&](int i) { return source[i]; });
    }

    T read(int index, const T& current) const {
        if (!valid(index)) {
            return current;
        }
        T result = current;
        const auto& entry = m_samples[index];
        Lerp{}(result, entry.previous, entry.current, Clock::get_interpolation_step());
        return result;
    }

    T read(int index, const T& current, float snap_distance) const {
        if (!valid(index) ||
            m_samples[index].previous.abs(m_samples[index].current) > snap_distance)
        {
            return current;
        }
        return read(index, current);
    }

private:
    bool valid(int index) const {
        return Clock::is_enabled() && Clock::is_presentation_active() && m_tick == Clock::sim_tick_seq() &&
               m_epoch == Clock::presentation_epoch() &&
               index >= 0 && static_cast<size_t>(index) < m_samples.size();
    }

    struct Entry {
        T previous;
        T current;
    };
    std::vector<Entry> m_samples;
    uint64_t m_tick = 0;
    uint64_t m_epoch = 0;
};

} // namespace gz::upstream
