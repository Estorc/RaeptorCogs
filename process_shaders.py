import sys
from pathlib import Path

def preprocess_shader(file_path, included_files=None, base_dir=None, system_include_dirs=None):
    if included_files is None:
        included_files = set()

    if base_dir is None:
        base_dir = file_path.parent
        
    if system_include_dirs is None:
        system_include_dirs = []  # Default to empty list for system includes

    content = ""
    for line in file_path.read_text().splitlines():
        if line.strip().startswith("#include"):
            include_directive = line.strip()
            
            # Handle #include "file.h" style
            if '"' in include_directive:
                include_path = include_directive.split('"')[1]
                full_include_path = (base_dir / include_path).resolve()
                include_type = "quoted"
            # Handle #include <file.h> style
            elif '<' in include_directive and '>' in include_directive:
                include_path = include_directive.split('<')[1].split('>')[0]
                # Try to find the file in system include directories
                full_include_path = None
                for dir_path in system_include_dirs:
                    potential_path = (Path(dir_path) / include_path).resolve()
                    if potential_path.exists():
                        full_include_path = potential_path
                        break
                if full_include_path is None:
                    # If not found in system paths, try base_dir as fallback
                    full_include_path = (base_dir / include_path).resolve()
                include_type = "angled"
            else:
                # Invalid include format, just add the line as is
                content += line + "\n"
                continue

            if not full_include_path.exists():
                content += f"// Warning: Could not find include file: {include_path}\n"
                content += line + "\n"
                continue
                
            if full_include_path in included_files:
                continue  # Avoid double include
                
            included_files.add(full_include_path)
            content += f"// Begin include {include_path}\n"
            content += preprocess_shader(full_include_path, included_files, base_dir, system_include_dirs)
            content += f"// End include {include_path}\n"
        else:
            content += line + "\n"
    return content

def main():
    out_path = Path(sys.argv[1])
    
    # Check if system include dirs are provided
    system_include_dirs = []
    remaining_args = []
    
    i = 2
    while i < len(sys.argv):
        if sys.argv[i] == "-I" and i + 1 < len(sys.argv):
            system_include_dirs.append(sys.argv[i+1])
            i += 2
        else:
            remaining_args.append(sys.argv[i])
            i += 1
    
    shader_files = [Path(arg) for arg in remaining_args]

    with out_path.open("w") as out_file:
        out_file.write("// Auto-generated header with embedded shaders\n\n")
        for shader_path in shader_files:
            var_name = shader_path.name.replace('.', '_')
            processed = preprocess_shader(shader_path, system_include_dirs=system_include_dirs)
            out_file.write(f'static const char* __shader__{var_name} = R"({processed})";\n\n')

if __name__ == "__main__":
    main()