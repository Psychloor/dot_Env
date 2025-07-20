# dot_env

A modern C++ library for loading and managing environment variables from `.env` files with support for type-safe access and endianness handling.

## Features

- Load environment variables from `.env` files
- Access environment variables with type safety
- Support for both system and local environment variables
- Cross-platform compatibility (Windows and Unix-like systems)
- Endianness-aware value retrieval (big-endian and little-endian)
- Modern C++23 implementation

## Requirements

- C++23 compatible compiler
- CMake 3.31 or higher

## Integration

### Option 1: FetchContent (Recommended)
```
cmake
include(FetchContent)
FetchContent_Declare(
dot_env
GIT_REPOSITORY https://github.com/Psychloor/dot_env.git
GIT_TAG v1.0.0  # Specify the tag/branch you want to use
)
FetchContent_MakeAvailable(dot_env)

# Link against the library
target_link_libraries(your_target PRIVATE dot_env::dot_env)
```
### Option 2: System Installation

Clone and build the library:
```
bash
git clone https://github.com/Psychloor/dot_env.git
cd dot_env
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build  # On Unix-like systems
# or
cmake --install build      # On Windows (run as administrator)
```
Then in your project's CMakeLists.txt:
```
cmake
find_package(dot_env REQUIRED)
target_link_libraries(your_target PRIVATE dot_env::dot_env)
```
### Option 3: Subdirectory

In your project's CMakeLists.txt:
```
cmake
add_subdirectory(path/to/dot_env)
target_link_libraries(your_target PRIVATE dot_env::dot_env)
```

## Build Configuration

### CMake Options

- `DOT_ENV_OVERRIDE_SYSTEM` (Default: OFF) - When enabled, allows variables from `.env` files to override existing system environment variables.

Example:
```
bash
# Configure with system override enabled
cmake -B build -DDOT_ENV_OVERRIDE_SYSTEM=ON

# Configure with system override disabled (default)
cmake -B build -DDOT_ENV_OVERRIDE_SYSTEM=OFF
```
## Environment Variable Handling

When accessing environment variables, the library follows this order:

### Lookup Priority
1. First checks internal variables (loaded from `.env` files)
2. Then checks system environment variables

### Loading Behavior
When loading from `.env` files, the behavior depends on the configuration:

#### With DOT_ENV_OVERRIDE_SYSTEM=OFF (Default):
- New variables from `.env` files are added to system environment
- Existing system variables are preserved

#### With DOT_ENV_OVERRIDE_SYSTEM=ON:
- All variables from `.env` files are added to system environment
- Can override existing system variables

You can also control this behavior at runtime:
```cpp
// Use compile-time default behavior
environment.load_env();

// Override system variables regardless of compile-time setting
environment.load_env(".env", true);

// Preserve system variables regardless of compile-time setting
environment.load_env(".env", false);
```

## Usage

### Basic Usage
```
cpp
#include <dot_env/env.hpp>

int main() {
dot_env::env environment;

    // Load variables from .env file
    environment.load_env();  // Defaults to ".env"
    // Or specify a custom file
    environment.load_env("custom.env");
    
    // Get a string value
    if (const std::string value = environment.get("DATABASE_URL")) {
        std::cout << "Database URL: " << *value << std::endl;
    }
    
    // Get a required value (throws if not found)
    try {
        std::string api_key = environment.require("API_KEY");
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
}
```
### Type-Safe Numeric Access
```
cpp
#include <dot_env/env.hpp>

int main() {
dot_env::env environment;
environment.load_env();

    // Get numeric values with specific endianness
    if (auto port = environment.get_le<int>("PORT")) {
        std::cout << "Port: " << *port << std::endl;
    }
    
    if (auto timeout = environment.get_be<double>("TIMEOUT")) {
        std::cout << "Timeout: " << *timeout << std::endl;
    }
}
```
## .env File Format
```
ini
# Comments are supported
STRING_VALUE=hello
QUOTED_VALUE="hello world"
NUMBER=42
FLOAT=3.14
```
## Environment Variable Handling

When loading variables from a `.env` file, the library handles them in the following ways:

### Loading Priority
1. First checks for existing system environment variables
2. Then loads from the `.env` file, which can override system variables if `override_system` is set to true

## License

MIT License

Copyright (c) 2025 Psychloor (Kevin Blomqvist)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

