#pragma once

#include <cmath>

namespace impl {

template <typename Item, typename FnGet>
float Average(const std::vector<Item>& items, const FnGet& fnGet)
{
    double sum = std::accumulate(std::begin(items), std::end(items), double(0), [&](double sum, const Item& x)
    {
        return sum + double(fnGet(x));
    });

    return sum / float(items.size());
}

template <typename Item, typename FnGet>
float Stddev(const std::vector<Item>& items, const float& average, const FnGet& fnGet)
{
    double accum = 0;
    std::for_each(std::begin(items), std::end(items), [&](const Item& x)
    {
        accum += (fnGet(x) - average) * (fnGet(x) - average);
    });

    return std::sqrt(accum / double(items.size() - 1));
}

template <typename Item, typename FnGet, typename FnSet>
void ZScore(std::vector<Item>& items, const FnGet& fnGet, const FnSet& fnSet)
{
    auto mean = impl::Average(items, fnGet);
    auto stddev = impl::Stddev(items, mean, fnGet);

    std::for_each(std::begin(items), std::end(items), [&](Item& x)
    {
        auto z = (fnGet(x) - mean) / stddev;
        fnSet(x, z);
    });
}

} // namespace impl

#define GET_ZSCORE(VEC, STAT, ZSTAT)                    \
        impl::ZScore(                                   \
            VEC,                                        \
            [](const auto& h)    { return h.STAT; },    \
            [](auto& h, float z) { h.ZSTAT = z; }       \
        );

