#pragma once
#include <vector>
#include <functional>
#include <string>
#include <array>
namespace RaeptorCogs {

using FileData = std::vector<unsigned char>;
using FileCallback = std::function<void(const RaeptorCogs::FileData&, const std::string&)>;
using FileDialogFilter = std::array<std::string, 2>;
using FileDialogFilters = std::vector<FileDialogFilter>;
FileData LoadFile(const char* filename);
void OpenFileDialog(FileCallback callback, const FileDialogFilters& filters = {});
bool LocalizeWorkingDirectory();

}