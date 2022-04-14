/**
 * @file Exceptions.hpp
 * @author Junaid Afzal
 * @brief Custom exceptions
 * @version 1.0
 * @date 14-04-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

namespace Exceptions
{
    class CocoNamesFileNotFound : public std::exception { };

    class InputVideoFileNotFound : public std::exception { };

    class OutputVideoFileNotCreated : public std::exception { };

    class OutOfFrames : public std::exception { };
}