/** ********************************************************************************
 * @section IO_Path_Overview Overview
 * @file Path.hpp
 * @brief High-level URL parsing utilities.
 * @details
 * Typical use cases:
 * - Parsing URLs into components
 * *********************************************************************************
 * @section IO_Path_Header Header
 * <RaeptorCogs/IO/Path.hpp>
 ***********************************************************************************
 * @section IO_Path_Metadata Metadata
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
#include <filesystem>

namespace RaeptorCogs {

/**
 * @brief URL parts structure.
 * 
 * Holds the domain and path components of a parsed URL.
 */
struct URLParts {
    /** The protocol part of the URL (e.g., "http", "https"). */
    std::string protocol;
    /** The domain part of the URL. */
    std::string domain;
    /** The path part of the URL. */
    std::string path;
};

/**
 * @brief Parse a URL into its components.
 * 
 * @param url The URL to parse.
 * @return structure containing the domain and path.
 * 
 * @code{.cpp}
 * RaeptorCogs::URLParts parts = RaeptorCogs::ParseURL("https://example.com/path/to/resource");
 * std::cout << "Domain: " << parts.domain << ", Path: " << parts.path << std::endl;
 * @endcode
 */
URLParts ParseURL(const std::string_view& url);

}