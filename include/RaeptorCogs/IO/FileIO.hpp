/** ********************************************************************************
 * @section IO_FileIO_Overview Overview
 * @file FileIO.hpp
 * @brief High-level utilities for loading files and opening native file dialogs.
 * @details
 * Typical use cases:
 * - Loading a file from disk:
 * - Opening a file dialog to select a file:
 * - Setting the working directory to the executable's directory:
 * *********************************************************************************
 * @section IO_FileIO_Header Header
 * <RaeptorCogs/IO/FileIO.hpp>
 ***********************************************************************************
 * @section IO_FileIO_Metadata Metadata
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
#include <vector>
#include <functional>
#include <string>
#include <array>
#include <filesystem>

namespace RaeptorCogs {

/**
 * @brief Raw file data type.
 * 
 * Represents the contents of a file as a vector of unsigned char.
 * 
 * @code{.cpp}
 * RaeptorCogs::FileData data = RaeptorCogs::LoadFile("example.txt");
 * @endcode
 */
using FileData = std::vector<unsigned char>;

/**
 * @brief Callback function type for file dialog operations.
 * 
 * This function is called when a file is selected in the file dialog.
 * It receives the file data and the file name as parameters.
 * 
 * @code{.cpp}
 * RaeptorCogs::OpenFileDialog([](const RaeptorCogs::FileData& data, const std::string& name) {
 *     // Handle the selected file data
 * });
 * @endcode
 */
using FileCallback = std::function<void(const RaeptorCogs::FileData&, const std::string&)>;

/**
 * @brief File dialog filter type.
 * 
 * Each filter is represented as an array of two strings:
 * the first string is the filter name (e.g., "Image Files"),
 * and the second string is the filter pattern (e.g., "*.png;*.jpg").
 * A collection of such filters is represented as a vector.
 * 
 * @code{.cpp}
 * RaeptorCogs::FileDialogFilter filter = {"Image Files", "*.png;*.jpg;*.jpeg"};
 * @endcode
 */
using FileDialogFilter = std::array<std::string, 2>;

/**
 * @brief File dialog filter collection.
 * 
 * Represents a vector of file dialog filters.
 * 
 * @code{.cpp}
 * RaeptorCogs::FileDialogFilters filters = {
 *  {"Image Files", "*.png;*.jpg;*.jpeg"},
 *  {"Text Files", "*.txt;*.md"},
 * };
 * @endcode
 */
using FileDialogFilters = std::vector<FileDialogFilter>;

/**
 * @brief Loads a file from disk into memory.
 *
 * Loads the specified file and returns its contents as a FileData object.
 *
 * @param filename  The path to the file to load.
 * @return The contents of the file as a FileData object.
 * @note The returned FileData is null-terminated, making it suitable for text files.
 */
FileData LoadFile(const std::filesystem::path& filename);

/**
 * @brief Opens a file dialog to select a file.
 * 
 * Opens a file dialog allowing the user to select a file. Once a file is selected,
 * the provided callback function is invoked with the file's data and name.
 * 
 * @param callback  The function to call with the selected file's data and name.
 * @param filters   Optional filters to apply to the file dialog.
 */
void OpenFileDialog(FileCallback callback, const FileDialogFilters& filters = {});

/**
 * @brief Sets the working directory to the executable's directory.
 * 
 * Sets the current working directory to the directory where the executable is located.
 * 
 * @return true if the working directory was successfully set, false otherwise.
 */
bool LocalizeWorkingDirectory();
}