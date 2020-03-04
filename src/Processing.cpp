#include <string>
#include <optional>
#include "FrameResult.h"
#include "Processing.h"

namespace hex
{
    Processing::Processing(std::optional<FrameResult> result, std::string colour, long duration)
        : result(result), colour(colour), duration(duration) { }
}
