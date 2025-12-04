#include <RaeptorCogs/IO/FileIO.hpp>
#include <RaeptorCogs/IO/String.hpp>
#include <RaeptorCogs/RaeptorCogs.hpp>
#include <iostream>
#include <fstream>
#include <map>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#else
#include <nfd.h>
#endif
#if defined(_WIN32)
    #include <windows.h>
    #include <direct.h>
#elif defined(__linux__) || defined(__APPLE__)
    #include <unistd.h>
    #include <limits.h>
    #include <libgen.h>
    #include <string.h>
#endif

namespace RaeptorCogs {
// Helper function to load a file into memory
FileData LoadFile(const std::filesystem::path& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return {};
    }  
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    if (size <= 0) {
        std::cerr << "File is empty or invalid: " << filename << std::endl;
        return {};
    }
    FileData buffer(static_cast<size_t>(size));
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    if (!file) {
        std::cerr << "Failed to read file: " << filename << std::endl;
        return {};
    }
    file.close();
    buffer.push_back('\0'); // Null-terminate the buffer
    return buffer;
}



#ifdef __EMSCRIPTEN__
std::map<int, FileCallback> g_fileCallbacks;
int g_nextCallbackID = 1;

extern "C" void file_callback_bridge(const unsigned char* data, int size, int callbackID, const char* name) {
    if (g_fileCallbacks.count(callbackID)) {
        RaeptorCogs::FileData buffer(data, data + size);
        g_fileCallbacks[callbackID](buffer, name);
        g_fileCallbacks.erase(callbackID);
    }
}

EM_JS(void, js_open_file_dialog, (int callbackID), {
    var input = document.createElement('input');
    input.type = 'file';
    input.onchange = function(event) {
        var file = event.target.files[0];
        var name = file.name;
        var reader = new FileReader();
        reader.onload = function(e) {
            var data = new Uint8Array(e.target.result);
            // Use ccall with typed array directly
            var buf = Module.HEAPU8.subarray(Module._malloc(data.length), Module._malloc(data.length) + data.length);
            buf.set(data);
            Module.ccall('file_callback_bridge', null, ['number','number','number','string'], [buf.byteOffset, data.length, callbackID, name]);
            Module._free(buf.byteOffset);
        };
        reader.readAsArrayBuffer(file);
    };
    input.click();
});

void OpenFileDialog(FileCallback callback, const FileDialogFilters& filters) {
    int id = g_nextCallbackID++;
    g_fileCallbacks[id] = std::move(callback);
    js_open_file_dialog(id);
}

#else
void OpenFileDialog(FileCallback callback, const FileDialogFilters& filters) {
    nfdu8char_t *outPath;
    std::vector<nfdu8filteritem_t> _filters;
    for (const auto& filter : filters) {
        _filters.push_back({ filter[0].c_str(), filter[1].c_str() });
    }
    nfdopendialogu8args_t args;
    args.defaultPath = nullptr;
    args.filterList = _filters.data();
    args.filterCount = static_cast<nfdfiltersize_t>(_filters.size());
    nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
        
    if ( result == NFD_OKAY ) {
        callback(LoadFile(outPath), outPath);
        NFD_FreePathU8(outPath);
    }
}
#endif

bool LocalizeWorkingDirectory() {
#if defined(_WIN32)
    char path[MAX_PATH];
    if (GetModuleFileNameA(NULL, path, MAX_PATH) == 0) return false;

    // Remove filename to get directory
    *strrchr(path, '\\') = '\0';
    return _chdir(path) == 0;

#elif defined(__linux__) || defined(__APPLE__)
    char path[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    if (count == -1) return false;
    path[count] = '\0';

    char *dir = dirname(path);
    return chdir(dir) == 0;

#else
    return false; // Not supported on unknown platform
#endif
}

}