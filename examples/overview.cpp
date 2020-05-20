#include <iostream>
#include <program_params/program_params.h>

int main (int argc, char *argv[])
{
    bool audible = false;
    size_t count = 10;
    float interval = 1.0f;
    std::string destination;

    program_params::Params params;
    params.add(audible, {"-a"});
    params.add(count, {"-c", "--count"});
    params.add(interval, {"-i", "--interval"});
    params.add(destination, {}, true);

    try
    {
        params.parse(argc - 1, argv + 1);
    }
    catch (const program_params::Exception &ex)
    {
        std::cout << ex.what() << std::endl;
        std::cout << "Usage:   overview [-a] [-c <count>] [-i <interval>] <destination>" << std::endl;
        std::cout << "Example: overview -a -c 10 -i 2.5 192.168.0.1" << std::endl;
        return 1;
    }

    std::cout << "Audible: " << audible << std::endl;
    std::cout << "Count: " << count << std::endl;
    std::cout << "Interval: " << interval << std::endl;
    std::cout << "Destination: " << destination << std::endl;
}
