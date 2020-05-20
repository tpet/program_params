/*
Created by Tomas Petricek on 5/6/20.

https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html

Note the ambiguities:
"-fbar" can be either four short options without arguments, or a single short
option with argument,
"-f bar" may be a short option with argument or a flag with a positional
argument.

What the arguments mean is thus application-specific and ambiguities are
resolved by the order of parameter declaration.
*/

#ifndef PROGRAM_PARAMS_PROGRAM_PARAMS_H
#define PROGRAM_PARAMS_PROGRAM_PARAMS_H

#include <cassert>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace program_params
{

class Exception: public std::runtime_error
{
public:
    Exception(const char *what):
            std::runtime_error(what)
    {}
};

bool is_option(const std::string &arg)
{
    return arg.size() > 0 && arg[0] == '-';
}

typedef std::string Str;
typedef std::vector<Str> StrVec;
typedef std::initializer_list<Str> StrInit;

class ParamBase
{
public:
    typedef std::shared_ptr<ParamBase> Ptr;

    ParamBase(const StrVec &names, bool required):
            names_(names), required_(required)
    {
        auto first = true;
        for (auto name: names)
        {
            auto option = is_option(name);
            assert(first || option == option_);
            option_ = option;
        }
    }

    virtual int parse(char **start, char **end) = 0;

    std::string value(char **start, char **end, int &inc) const
    {
        assert(start < end);
        std::string opt(start[0]);
        if (!option_)
        {
            // Positional, we can directly return the value.
            inc = 1;
            return opt;
        }
        // An option, possibly combined with value in the same token.
        auto i = opt.find('=');
        if (i != std::string::npos)
        {
            // Long option with equals delimiter.
            inc = 1;
            return opt.substr(i + 1);
        }
        if (opt.size() > 2)
        {
            // Short option with appended value.
            inc = 1;
            return opt.substr(2);
        }
        // Value as a separate token.
        assert(start + 1 < end);
        inc = 2;
        return std::string(start[1]);
    }

    void check() const
    {
        if (required_ && !found_)
        {
            throw Exception("Required parameter not found.");
        }
    }
protected:
    const StrVec names_;
    bool option_;
    bool required_;
    bool found_;
};

template<typename T>
class Param: public ParamBase
{
public:
    Param(T &target, const StrVec &names, bool replace):
            ParamBase(names, replace), target_(target)
    {}
    virtual int parse(char **start, char **end);
    T &target_;
};

template<>
int Param<bool>::parse(char **start, char **end)
{
    assert(start < end);
    target_ = true;
    found_ = true;
    return 0;
}

template<>
int Param<std::string>::parse(char **start, char **end)
{
    assert(start < end);
    int inc = 0;
    target_ = value(start, end, inc);
    found_ = true;
    return inc;
}

template<>
int Param<int>::parse(char **start, char **end)
{
    assert(start < end);
    int inc = 0;
    target_ = std::stoi(value(start, end, inc));
    found_ = true;
    return inc;
}

template<>
int Param<unsigned int>::parse(char **start, char **end)
{
    assert(start < end);
    int inc = 0;
    target_ = std::stoul(value(start, end, inc));
    found_ = true;
    return inc;
}

template<>
int Param<long>::parse(char **start, char **end)
{
    assert(start < end);
    int inc = 0;
    target_ = std::stol(value(start, end, inc));
    found_ = true;
    return inc;
}

template<>
int Param<unsigned long>::parse(char **start, char **end)
{
    assert(start < end);
    int inc = 0;
    target_ = std::stoul(value(start, end, inc));
    found_ = true;
    return inc;
}

template<>
int Param<float>::parse(char **start, char **end)
{
    assert(start < end);
    int inc = 0;
    target_ = std::stof(value(start, end, inc));
    found_ = true;
    return inc;
}

template<>
int Param<double>::parse(char **start, char **end)
{
    assert(start < end);
    int inc = 0;
    target_ = std::stod(value(start, end, inc));
    found_ = true;
    return inc;
}

class ValueBase
{
public:
    typedef std::shared_ptr<ValueBase> Ptr;
};

template<typename T>
class Value: public ValueBase
{
public:
    T value_;
};

class Params
{
public:
    typedef std::unordered_map<std::string, ParamBase::Ptr> Map;
    typedef std::vector<ParamBase::Ptr> Vec;

    Params(bool strict = true):
            strict_(strict)
    {}

    template<typename T>
    void add(T &target, StrVec names, bool required = false)
    {
        auto ptr = std::make_shared<Param<T>>(target, names, required);
        bool option = false;
        for (auto name: names)
        {
            map_[name] = ptr;
            if (is_option(name))
            {
                option = true;
            }
            else
            {
                assert(!option);
            }
        }
        if (!option)
        {
            positional_.push_back(ptr);
        }
    }
    template<typename T>
    void add(T &target, StrInit names, bool required = false)
    {
        add(target, StrVec(names), required);
    }
    template<typename T>
    void add(StrVec names, bool required = false)
    {
        auto ptr = std::make_shared<Value<T>>();
        values_.push_back(ptr);
        add(ptr->value_, StrVec(names), required);
    }
    template<typename T>
    void add(StrInit names, bool required = false)
    {
        add<T>(StrVec(names), required);
    }
    template<typename T>
    T & get(const Str &name)
    {
        auto it = map_.find(name);
        if (it == map_.end())
        {
            throw Exception("Parameter not found.");
        }
        Param<T> *param = dynamic_cast<Param<T> *>(it->second.get());
        if (!param)
        {
            throw Exception("Conversion not supported.");
        }
        return param->target_;
    }
    void parse(int argc, char **argv)
    {
        auto next = positional_.begin();
        bool positional_onward = false;
        for (char **start = argv, **end = argv + argc; start < end;)
        {
            const std::string arg(start[0]);
            if (positional_onward
                    || arg.empty()
                    || arg == "-"
                    || arg[0] != '-')
            {
                if (next < positional_.end())
                {
                    start += (*next)->parse(start, end);
                    ++next;
                }
                else if (strict_)
                {
                    throw Exception("Unknown positional parameter.");
                }
                continue;
            }
            if (arg == "--")
            {
                // The argument ‘--’ terminates all options; any following
                // arguments are treated as non-option arguments, even if they
                // begin with a hyphen.
                positional_onward = true;
                ++start;
                continue;
            }
            if (arg[1] != '-')
            {
                // Short option(s).
                // Combined options in POSIX must not take arguments.
                int inc;
                for (size_t i = 1; i < arg.size(); ++i)
                {
                    std::string opt = "-" + arg.substr(i, 1);
                    auto it = map_.find(opt);
                    if (it != map_.end())
                    {
                        if (i > 1)
                        {
                            // TODO: Check option has no arguments.
                        }
                        inc = it->second->parse(start, end);
                        if (inc > 0)
                        {
                            // Value parsed, either as a part of option token
                            // or a separate token(s).
                            break;
                        }
                    }
                    else if (strict_)
                    {
                        throw Exception("Unknown short option.");
                    }
                }
                // Increment from parsing, or 1 if no value was encountered.
                start += inc > 0 ? inc : 1;
            }
            else
            {
                // Long option.
                auto i = arg.find('=');
                auto opt = arg.substr(0, i);
                auto it = map_.find(opt);
                if (it != map_.end())
                {
                    start += it->second->parse(start, end);
                }
                else if (strict_)
                {
                    throw Exception("Unknown long option.");
                }
            }
        }
        for (const auto &p: map_)
        {
            p.second->check();
        }
        for (const auto &p: positional_)
        {
            p->check();
        }
    }
protected:
    bool strict_;
    Map map_;
    Vec positional_;
    std::vector<ValueBase::Ptr> values_;
};

}

#endif //PROGRAM_PARAMS_PROGRAM_PARAMS_H
