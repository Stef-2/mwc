import mtr_string;

import std;

int main()
{
    mtr::string_t wtf = {"asd"};
    std::println("ait");
    
    std::println("{0}", wtf);

    std::println("{0}", std::stacktrace::current());
}