#pragma once

#include <cmath>

namespace impl {

template <typename Item, typename FnGet>
double Mean(const std::vector<Item>& items, size_t count, const FnGet& fnGet)
{
    double sum = 0;
    for (size_t i = 0; i < count; i++) {
        sum += double(fnGet(items[i]));
    }

    return sum / double(count);
}

template <typename Item, typename FnGet>
float Stddev(const std::vector<Item>& items, size_t count, const float& average, const FnGet& fnGet)
{
    double sum = 0;

    for (size_t i = 0; i < count; i++) {
        double temp = double(fnGet(items[i]) - average);
        sum += (temp * temp);
    }

    return std::sqrt(sum / double(count - 1));
}

template <typename Item, typename FnGet, typename FnSet>
void ZScore(std::vector<Item>& items, size_t count, const FnGet& fnGet, const FnSet& fnSet)
{
    auto mean = impl::Mean(items, count, fnGet);
    auto stddev = impl::Stddev(items, count, mean, fnGet);

    std::vector<double> temp;

    std::for_each(std::begin(items), std::end(items), [&](Item& x)
    {
        // auto z = (fnGet(x) - mean) / (stddev / std::sqrt(count));
        auto z = (fnGet(x) - mean) / stddev;
        fnSet(x, z);
        temp.push_back(z);
    });

    /*
    for (int32_t i = 0; i < int32_t(count); i++)
    {
        double sum = 0;
        double cSum = 0;

        // Savitzky-Golay filters
        std::vector<int32_t> C = { 179, 135, 30, -55, 15 };
        // std::vector<int32_t> C = { 7, 6, 3, -2 };
        // std::vector<int32_t> C = { 59, 54, 39, 14, -21 };

        for (size_t j = 0; j < C.size(); j++) {

            auto InBounds = [&](int32_t i) -> bool {
                return i >= 0 && i < int32_t(count);
            };

            int32_t idx = i + j;
            if (InBounds(idx)) {
                sum += temp[idx] * C[j];
                cSum += C[j];
            }

            if (j != 0) {
                int32_t idx = i - j;
                if (InBounds(idx)) {
                    sum += temp[idx] * C[j];
                    cSum += C[j];
                }
            }
        }

        auto smooth = sum / cSum;
        fnSet(items[i], smooth);
    }
    */
}

} // namespace impl

#define GET_ZSCORE(VEC, COUNT, STAT, ZSTAT)             \
        impl::ZScore(                                   \
            VEC,                                        \
            COUNT,                                      \
            [](const auto& h)    { return h.STAT; },    \
            [](auto& h, float z) { h.ZSTAT = z; }       \
        );

