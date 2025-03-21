module;

export module mtr_output_stream;

import std;

export namespace mtr
{
    namespace output
    {
        auto output_stream() -> const std::ostream&
        {
            return std::cout;
        }

        auto error_stream() -> const std::stream&
        {
            return std::cerr;
        }

        auto log_stream() -> const std::stream&
        {
            return std::clog;
        }
    }
}