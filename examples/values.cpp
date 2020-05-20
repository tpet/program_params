#include <iostream>
#include <program_params/program_params.h>

int main (int argc, char *argv[])
{
    program_params::Params params;
    params.add<bool>({"-a"});
    params.add<size_t>({"-c", "--count"});
    params.add<float>({"-i", "--interval"});
    params.add<std::string>({"destination"}, true);

    try
    {
        params.parse(argc - 1, argv + 1);
    }
    catch (const program_params::Exception &ex)
    {
        std::cout << ex.what() << std::endl;
        std::cout << "Usage:   values [-a] [-c <count>] [-i <interval>] <destination>" << std::endl;
        std::cout << "Example: values -a -c 10 -i 2.5 192.168.0.1" << std::endl;
        return 1;
    }

    std::cout << "Audible: " << params.get<bool>("-a") << std::endl;
    std::cout << "Count: " << params.get<size_t>("--count") << std::endl;
    std::cout << "Interval: " << params.get<float>("--interval") << std::endl;
    std::cout << "Destination: " << params.get<std::string>("destination") << std::endl;
}
