/** ********************************************************************************
 * @section Measure_Overview Overview
 * @file Measure.hpp
 * @brief High-level measurement utilities.
 * @details
 * Typical use cases:
 * - Measuring execution time of code blocks.
 * *********************************************************************************
 * @section Measure_Header Header
 * <RaeptorCogs/Measure.hpp>
 ***********************************************************************************
 * @section Measure_Metadata Metadata
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
#include <chrono>
#include <iostream>

/*
class Measure {
public:
    Measure() : start(std::chrono::high_resolution_clock::now()) {}

    ~Measure() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "Duration: " << duration << " ms" << std::endl;
    }

private:
    std::chrono::high_resolution_clock::time_point start;
};

class GPUMeasure {
public:
    GPUMeasure() {
        #ifndef __EMSCRIPTEN__
        glGenQueries(1, &query);
        glBeginQuery(GL_TIME_ELAPSED, query);
        #endif
    }

    ~GPUMeasure() {
        #ifndef __EMSCRIPTEN__
        glEndQuery(GL_TIME_ELAPSED);
        GLuint64 timeElapsed;
        glGetQueryObjectui64v(query, GL_QUERY_RESULT, &timeElapsed);
        glDeleteQueries(1, &query);
        std::cout << "GPU Duration: " << timeElapsed / 1e6 << " ms" << std::endl;
        #endif
    }
private:
    GLuint query;
};
*/