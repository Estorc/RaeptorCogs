/** ********************************************************************************
 * @section Time_Overview Overview
 * @file Time.hpp
 * @brief High-level time handling utilities.
 * @details
 * Typical use cases:
 * - Managing time measurement and frame timing.
 * *********************************************************************************
 * @section Time_Header Header
 * <RaeptorCogs/Time.hpp>
 ***********************************************************************************
 * @section Time_Metadata Metadata
 * @author Estorc
 * @version v1.0
 * @copyright Copyright (c) 2025 Estorc MIT License.
 **********************************************************************************/
/*                             This file is part of
 *                                  RaeptorCogs
 *                     (https://github.com/Estorc/RaeptorCogs)
 ***********************************************************************************
 * Copyright (c) 2025 Estorc.
 * This file is licensed under the MIT License.
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ***********************************************************************************/

#pragma once
#include <RaeptorCogs/Singleton.hpp>

namespace RaeptorCogs::Singletons {

    /**
     * @brief Time singleton class.
     * 
     * Manages time measurement and frame timing.
     */
    class Time {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Delta time between frames in seconds.
         * 
         * Used for frame-independent movement and updates.
         */
        double deltaTime;
        
        /**
         * @brief Time at the last frame in seconds.
         * 
         * Used to compute delta time.
         */
        double lastFrameTime;

        /**
         * @brief Default constructor.
         */
        Time() = default;

        /**
         * @brief Default destructor.
         */
        ~Time() = default;
        friend SingletonAccessor<Time>;
    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Compute the delta time between frames.
         * 
         * Updates the deltaTime attribute based on the current time.
         */
        void computeDeltaTime();

        /**
         * @brief Get the delta time between frames.
         * 
         * @return double The delta time in seconds.
         */
        double getPreciseDeltaTime();

        /**
         * @brief Get the current time in seconds.
         * 
         * @return double The current time in seconds.
         */
        double getPreciseTime();

        /**
         * @brief Get the delta time between frames.
         * 
         * @return float The delta time in seconds.
         */
        float getDeltaTime();

        /**
         * @brief Get the current time in seconds.
         * 
         * @return float The current time in seconds.
         */
        float getTime();
    };
}
