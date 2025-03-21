module;

export module mtr_debug;

export namespace mtr
{
    consteval auto debugging()
    {
        #ifdef MTR_DEBUG
            return true;
        #else
            return false;
        #endif
    }
}