#pragma once
#include <string>
#include <optional>
#include "FrameResult.h"

namespace hex
{
    struct Processing
    {
        std::optional<FrameResult> result;
        std::string colour;
        long duration;

    public:
        Processing(std::optional<FrameResult> result, std::string colour, long duration);
        Processing() = default;
    };
}

