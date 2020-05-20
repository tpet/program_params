
# Program Params

Header-only C++ library for parsing program parameters from command-line arguments.

## Usage

### Overview

```C++
#include <program_params/program_params.h>

int main (int argc, char *argv[])
{
    size_t count = 10;
    float interval = 1.0f;
    std::string destination;
    
    program_params::Params params;
    params.add(count, {"-c", "--count"});
    params.add(interval, {"-i", "--interval"});
    params.add(destination, {}, true);
    try
    {
        params.parse(argc - 1, argv + 1);
    }
    catch (const params::Exception &ex)
    {
        // Print usage and exit.
    }
    // Do something useful with your params.
}
```

### Adding Parameter

Using external storage (type is inferred):
```C++
params.add(storage, names, required)
```

Using internal storage (type must be specified):
```C++
params.add<T>(names, required)
```

Positional parameters may have no name:
```C++
params.add(storage, {}, required)
```

### Reading Parameter

```C++
params.get<T>(name)
```

Parameters with external storage are stored directly to the provided storage.

### Features

- Short and long options, allowing multiple names:
  ```C++
  params.add(count, {"-c"});
  params.add(interval, {"--interval"});
  params.add(size, {"-s", "--size"});
  ```
  
- Positional parameters (non-options) with optional name:<br>
  ```C++
  params.add(destination, {});
  params.add(destination, {"destination"});
  params.add<std::string>({"destination"});
  ```
  Name needed to retrieve the value with internal storage.
  
- Required options or positional parameters:
  ```C++
  params.add(count, {"-c"}, true);
  params.add(destination, {}, true);
  ```
  
- Inferring type for external storage:<br>
  ```C++
  bool flag;
  int count;
  params.add(flag, {"-f"});
  params.add(count, {"-c"});
  ```
  
- Internal storage with explicit type:<br>
  ```C++
  params.add<size_t>({"-c", "--count"});
  params.add<std::string>({"destination"});
  // Parse params.
  params.get<size_t>({"-c"});
  params.get<size_t>({"--count"});
  params.get<std::string>({"destination"});
  ```
  
- Strict argument checking (doesn't allow unknown arguments):
  ```C++
  program_params::Params(true);
  ```
  
- Unknown arguments allowed by default:
  ```C++
  program_params::Params(false);
  program_params::Params();
  ```
- Short options without values (flags) in single argument:<br>
  ```C++
  params.add<bool>({"-a"});
  params.add<bool>({"-s"});
  params.add<bool>({"-d"});
  params.add<bool>({"-f"});
  ```
  ```bash
  program -asdf
  ```
- Short option with value in single argument:
  ```bash
  program -i2.5
  ```
- Long option with value in single argument:
  ```bash
  program --interval=2.5
  ```
- Long option with value as separate arguments:
  ```bash
  program --interval 2.5
  ```

# Alternatives

- [GNU Getopt](https://www.gnu.org/software/libc/manual/html_node/Getopt.html)
- [Google Gflags](https://github.com/gflags/gflags)
- [Boost Program Options](https://www.boost.org/doc/libs/release/libs/program_options/)
- [A Simple C++ Argument Parser](http://graphics.stanford.edu/~drussel/Argument_helper/)
