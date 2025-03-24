module;

export module mtr_assert;

import mtr_debug;
import mtr_log;

import std;

export namespace mtr
{
    template <typename T>
    concept convertible_to_bool = requires (T a_type) {a_type == true;} and std::convertible_to<T, bool>;

    //std::convertible_to<T, bool>::value and (T a_type) {if consteval {a_type == true;}}

    template <typename T>
    requires convertible_to_bool<T>
    constexpr auto mtr_assert(T a_condition) -> void
    {
        if constexpr (not debugging())
        return;

        if consteval
        {
            if constexpr (std::is_same_v<T, bool>)
                static_assert(a_condition == true);
            /*if (not a_condition)
            {
                //log::error("assertion failed");
                //std::terminate();
                static_assert(false);
            }*/
        }/*
        else

        if (not a_condition)
        {
            //log::error("assertion failed");
            std::terminate();
        }*/
    }

    constexpr auto wtf = false;

    //mtr_assert<bool>(true);

    int test()
    {
        static constexpr auto a = false;
        //mtr_assert<false>(a);
        bool x = true;
        mtr_assert(true);
        return 0;
    }
}