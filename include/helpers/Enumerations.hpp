/**
 * @file Enumerations.hpp
 * @author Junaid Afzal
 * @brief Custom enumerations
 * @version 1.0
 * @date 14-04-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

namespace Enumerations
{
    enum class Detector { NONE=0, STANDARD=1, TINY=2 };

    enum class BackEnd { CPU=0, CUDA=1 };

    enum class BlobSize { ONE=288, TWO=320, THREE=416, FOUR=512, FIVE=608 };
}
