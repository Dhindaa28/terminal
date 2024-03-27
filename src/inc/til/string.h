// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

namespace til // Terminal Implementation Library. Also: "Today I Learned"
{
    _TIL_INLINEPREFIX std::wstring visualize_control_codes(std::wstring str) noexcept
    {
        for (auto& ch : str)
        {
            if (ch < 0x20)
            {
                ch += 0x2400;
            }
            else if (ch == 0x20)
            {
                ch = 0x2423; // replace space with ␣
            }
            else if (ch == 0x7f)
            {
                ch = 0x2421; // replace del with ␡
            }
        }
        return str;
    }

    _TIL_INLINEPREFIX std::wstring visualize_control_codes(std::wstring_view str)
    {
        return visualize_control_codes(std::wstring{ str });
    }

    namespace details
    {
        inline constexpr uint8_t __ = 0b00;
        inline constexpr uint8_t F_ = 0b10; // stripped in clean_filename
        inline constexpr uint8_t _P = 0b01; // stripped in clean_path
        inline constexpr uint8_t FP = 0b11; // stripped in clean_filename and clean_path
        inline constexpr std::array<uint8_t, 128> pathFilter{ {
            // clang-format off
            __ /* NUL */, __ /* SOH */, __ /* STX */, __ /* ETX */, __ /* EOT */, __ /* ENQ */, __ /* ACK */, __ /* BEL */, __ /* BS  */, __ /* HT  */, __ /* LF  */, __ /* VT  */, __ /* FF  */, __ /* CR  */, __ /* SO  */, __ /* SI  */,
            __ /* DLE */, __ /* DC1 */, __ /* DC2 */, __ /* DC3 */, __ /* DC4 */, __ /* NAK */, __ /* SYN */, __ /* ETB */, __ /* CAN */, __ /* EM  */, __ /* SUB */, __ /* ESC */, __ /* FS  */, __ /* GS  */, __ /* RS  */, __ /* US  */,
            __ /* SP  */, __ /* !   */, FP /* "   */, __ /* #   */, __ /* $   */, __ /* %   */, __ /* &   */, __ /* '   */, __ /* (   */, __ /* )   */, FP /* *   */, __ /* +   */, __ /* ,   */, __ /* -   */, __ /* .   */, F_ /* /   */,
            __ /* 0   */, __ /* 1   */, __ /* 2   */, __ /* 3   */, __ /* 4   */, __ /* 5   */, __ /* 6   */, __ /* 7   */, __ /* 8   */, __ /* 9   */, F_ /* :   */, __ /* ;   */, FP /* <   */, __ /* =   */, FP /* >   */, FP /* ?   */,
            __ /* @   */, __ /* A   */, __ /* B   */, __ /* C   */, __ /* D   */, __ /* E   */, __ /* F   */, __ /* G   */, __ /* H   */, __ /* I   */, __ /* J   */, __ /* K   */, __ /* L   */, __ /* M   */, __ /* N   */, __ /* O   */,
            __ /* P   */, __ /* Q   */, __ /* R   */, __ /* S   */, __ /* T   */, __ /* U   */, __ /* V   */, __ /* W   */, __ /* X   */, __ /* Y   */, __ /* Z   */, __ /* [   */, F_ /* \   */, __ /* ]   */, __ /* ^   */, __ /* _   */,
            __ /* `   */, __ /* a   */, __ /* b   */, __ /* c   */, __ /* d   */, __ /* e   */, __ /* f   */, __ /* g   */, __ /* h   */, __ /* i   */, __ /* j   */, __ /* k   */, __ /* l   */, __ /* m   */, __ /* n   */, __ /* o   */,
            __ /* p   */, __ /* q   */, __ /* r   */, __ /* s   */, __ /* t   */, __ /* u   */, __ /* v   */, __ /* w   */, __ /* x   */, __ /* y   */, __ /* z   */, __ /* {   */, FP /* |   */, __ /* }   */, __ /* ~   */, __ /* DEL */,
            // clang-format on
        } };
    }

    _TIL_INLINEPREFIX std::wstring clean_filename(std::wstring str) noexcept
    {
        using namespace til::details;
        std::erase_if(str, [](auto ch) {
            // This lookup is branchless: It always checks the filter, but throws
            // away the result if ch >= 128. This is faster than using `&&` (branchy).
            return ((til::at(details::pathFilter, ch & 127) & F_) != 0) & (ch < 128);
        });
        return str;
    }

    _TIL_INLINEPREFIX std::wstring clean_path(std::wstring str) noexcept
    {
        using namespace til::details;
        std::erase_if(str, [](auto ch) {
            return ((til::at(details::pathFilter, ch & 127) & _P) != 0) & (ch < 128);
        });
        return str;
    }

    // is_legal_path rules on whether a path contains any non-path characters.
    // it **DOES NOT** rule on whether a path exists.
    _TIL_INLINEPREFIX constexpr bool is_legal_path(const std::wstring_view str) noexcept
    {
        using namespace til::details;
        return !std::any_of(std::begin(str), std::end(str), [](auto&& ch) {
            return ((til::at(details::pathFilter, ch & 127) & _P) != 0) & (ch < 128);
        });
    }

    // std::string_view::starts_with support for C++17.
    template<typename T, typename Traits>
    constexpr bool starts_with(const std::basic_string_view<T, Traits>& str, const std::basic_string_view<T, Traits>& prefix) noexcept
    {
        return str.size() >= prefix.size() && __builtin_memcmp(str.data(), prefix.data(), prefix.size() * sizeof(T)) == 0;
    }

    constexpr bool starts_with(const std::string_view& str, const std::string_view& prefix) noexcept
    {
        return starts_with<>(str, prefix);
    }

    constexpr bool starts_with(const std::wstring_view& str, const std::wstring_view& prefix) noexcept
    {
        return starts_with<>(str, prefix);
    }

    // std::string_view::ends_with support for C++17.
    template<typename T, typename Traits>
    constexpr bool ends_with(const std::basic_string_view<T, Traits>& str, const std::basic_string_view<T, Traits>& suffix) noexcept
    {
#pragma warning(suppress : 26481) // Don't use pointer arithmetic. Use span instead (bounds.1).
        return str.size() >= suffix.size() && __builtin_memcmp(str.data() + (str.size() - suffix.size()), suffix.data(), suffix.size() * sizeof(T)) == 0;
    }

    constexpr bool ends_with(const std::string_view& str, const std::string_view& prefix) noexcept
    {
        return ends_with<>(str, prefix);
    }

    constexpr bool ends_with(const std::wstring_view& str, const std::wstring_view& prefix) noexcept
    {
        return ends_with<>(str, prefix);
    }

    namespace details
    {
        // Just like std::wcstoul, but without annoying locales and null-terminating strings.
        // It has been fuzz-tested against clang's strtoul implementation.
        template<typename T, typename Traits>
        _TIL_INLINEPREFIX constexpr std::optional<uint64_t> parse_u64(const std::basic_string_view<T, Traits>& str, int base = 0) noexcept
        {
            // We don't have to test ptr for nullability, as we only access it under either condition:
            // * str.length() > 0, for determining the base
            // * ptr != end, when parsing the characters; if ptr is null, length will be 0 and thus end == ptr
#pragma warning(push)
#pragma warning(disable : 26429) // Symbol 'ptr' is never tested for nullness, it can be marked as not_null
#pragma warning(disable : 26481) // Don't use pointer arithmetic. Use span instead
            auto ptr = str.data();
            const auto end = ptr + str.length();
            uint64_t accumulator = 0;
            uint64_t value = 0;

            if (!base)
            {
                base = 10;

                if (str.length() > 1 && *ptr == '0')
                {
                    base = 8;
                    ++ptr;

                    if (str.length() > 2 && (*ptr == 'x' || *ptr == 'X'))
                    {
                        base = 16;
                        ++ptr;
                    }
                }
            }

            if (ptr == end)
            {
                return {};
            }

            for (;;)
            {
                if (*ptr >= '0' && *ptr <= '9')
                {
                    value = *ptr - '0';
                }
                else if (*ptr >= 'A' && *ptr <= 'F')
                {
                    value = *ptr - 'A' + 10;
                }
                else if (*ptr >= 'a' && *ptr <= 'f')
                {
                    value = *ptr - 'a' + 10;
                }
                else
                {
                    return {};
                }

                const auto acc = accumulator * base + value;
                if (acc < accumulator)
                {
                    return {};
                }
                accumulator = acc;

                if (++ptr == end)
                {
                    return accumulator;
                }
            }
#pragma warning(pop)
        }

        template<std::unsigned_integral R, typename T, typename Traits>
        constexpr std::optional<R> parse_unsigned(const std::basic_string_view<T, Traits>& str, int base = 0) noexcept
        {
            if constexpr (std::is_same_v<R, uint64_t>)
            {
                return details::parse_u64<>(str, base);
            }
            else
            {
                const auto opt = details::parse_u64<>(str, base);
                if (!opt || *opt > uint64_t{ std::numeric_limits<R>::max() })
                {
                    return {};
                }
                return gsl::narrow_cast<R>(*opt);
            }
        }

        template<std::signed_integral R, typename T, typename Traits>
        constexpr std::optional<R> parse_signed(std::basic_string_view<T, Traits> str, int base = 0) noexcept
        {
            const bool hasSign = str.starts_with(L'-');
            if (hasSign)
            {
                str = str.substr(1);
            }

            const auto opt = details::parse_u64<>(str, base);
            if (!opt || *opt > uint64_t{ std::numeric_limits<R>::max() })
            {
                return {};
            }

            const auto r = gsl::narrow_cast<R>(*opt);
            return hasSign ? -r : r;
        }
    }

    template<typename R>
    constexpr std::optional<R> parse_unsigned(const std::string_view& str, int base = 0) noexcept
    {
        return details::parse_unsigned<R>(str, base);
    }

    template<typename R>
    constexpr std::optional<R> parse_unsigned(const std::wstring_view& str, int base = 0) noexcept
    {
        return details::parse_unsigned<R>(str, base);
    }

    template<typename R>
    constexpr std::optional<R> parse_signed(const std::string_view& str, int base = 0) noexcept
    {
        return details::parse_signed<R>(str, base);
    }

    template<typename R>
    constexpr std::optional<R> parse_signed(const std::wstring_view& str, int base = 0) noexcept
    {
        return details::parse_signed<R>(str, base);
    }

    // Implement to_int in terms of to_ulong by negating its result. to_ulong does not expect
    // to be passed signed numbers and will return an error accordingly. That error when
    // compared against -1 evaluates to true. We account for that by returning to_int_error if to_ulong
    // returns an error.
    constexpr int to_int(const std::wstring_view& str, unsigned long base = 0) noexcept
    {
        auto result = to_ulong_error;
        const auto signPosition = str.find(L"-");
        const bool hasSign = signPosition != std::wstring_view::npos;
        result = hasSign ? to_ulong(str.substr(signPosition + 1), base) : to_ulong(str, base);

        // Check that result is valid and will fit in an int.
        if (result == to_ulong_error || (result > INT_MAX))
        {
            return to_int_error;
        }

        return hasSign ? result * -1 : result;
    }

    // Just like std::tolower, but without annoying locales.
    template<typename T>
    constexpr T tolower_ascii(T c)
    {
        if ((c >= 'A') && (c <= 'Z'))
        {
            c |= 0x20;
        }

        return c;
    }

    // Just like std::toupper, but without annoying locales.
    template<typename T>
    constexpr T toupper_ascii(T c)
    {
        if ((c >= 'a') && (c <= 'z'))
        {
            c &= ~0x20;
        }

        return c;
    }

    // Just like std::wstring_view::operator==().
    //
    // At the time of writing wmemcmp() is not an intrinsic for MSVC,
    // but the STL uses it to implement wide string comparisons.
    // This produces 3x the assembly _per_ comparison and increases
    // runtime by 2-3x for strings of medium length (16 characters)
    // and 5x or more for long strings (128 characters or more).
    // See: https://github.com/microsoft/STL/issues/2289
    template<typename T, typename Traits>
    bool equals(const std::basic_string_view<T, Traits>& lhs, const std::basic_string_view<T, Traits>& rhs) noexcept
    {
        return lhs.size() == rhs.size() && __builtin_memcmp(lhs.data(), rhs.data(), lhs.size() * sizeof(T)) == 0;
    }

    // Just like _memicmp, but without annoying locales.
    template<typename T, typename Traits>
    bool equals_insensitive_ascii(const std::basic_string_view<T, Traits>& str1, const std::basic_string_view<T, Traits>& str2) noexcept
    {
        if (str1.size() != str2.size())
        {
            return false;
        }

#pragma warning(push)
#pragma warning(disable : 26429) // Symbol 'data1' is never tested for nullness, it can be marked as not_null
#pragma warning(disable : 26481) // Don't use pointer arithmetic. Use span instead
        auto remaining = str1.size();
        auto data1 = str1.data();
        auto data2 = str2.data();
        for (; remaining; --remaining, ++data1, ++data2)
        {
            if (*data1 != *data2 && tolower_ascii(*data1) != tolower_ascii(*data2))
            {
                return false;
            }
        }
#pragma warning(pop)

        return true;
    }

    inline bool equals_insensitive_ascii(const std::string_view& str1, const std::string_view& str2) noexcept
    {
        return equals_insensitive_ascii<>(str1, str2);
    }

    inline bool equals_insensitive_ascii(const std::wstring_view& str1, const std::wstring_view& str2) noexcept
    {
        return equals_insensitive_ascii<>(str1, str2);
    }

    template<typename T, typename Traits>
    constexpr bool starts_with_insensitive_ascii(const std::basic_string_view<T, Traits>& str, const std::basic_string_view<T, Traits>& prefix) noexcept
    {
        return str.size() >= prefix.size() && equals_insensitive_ascii<>({ str.data(), prefix.size() }, prefix);
    }

    constexpr bool starts_with_insensitive_ascii(const std::string_view& str, const std::string_view& prefix) noexcept
    {
        return starts_with_insensitive_ascii<>(str, prefix);
    }

    constexpr bool starts_with_insensitive_ascii(const std::wstring_view& str, const std::wstring_view& prefix) noexcept
    {
        return starts_with_insensitive_ascii<>(str, prefix);
    }

    template<typename T, typename Traits>
    constexpr bool ends_with_insensitive_ascii(const std::basic_string_view<T, Traits>& str, const std::basic_string_view<T, Traits>& suffix) noexcept
    {
#pragma warning(suppress : 26481) // Don't use pointer arithmetic. Use span instead (bounds.1).
        return str.size() >= suffix.size() && equals_insensitive_ascii<>({ str.data() - suffix.size(), suffix.size() }, suffix);
    }

    constexpr bool ends_with_insensitive_ascii(const std::string_view& str, const std::string_view& prefix) noexcept
    {
        return ends_with_insensitive_ascii<>(str, prefix);
    }

    constexpr bool ends_with_insensitive_ascii(const std::wstring_view& str, const std::wstring_view& prefix) noexcept
    {
        return ends_with<>(str, prefix);
    }

    template<typename T, typename Traits>
    struct split_iterator
    {
        struct sentinel
        {
        };

        struct iterator
        {
            using iterator_category = std::forward_iterator_tag;
            using value_type = std::basic_string_view<T, Traits>;
            using reference = const value_type&;
            using pointer = const value_type*;
            using difference_type = std::ptrdiff_t;

            explicit constexpr iterator(split_iterator& p) noexcept :
                _iter{ p }
            {
            }

            const value_type& operator*() const noexcept
            {
                return _iter.value();
            }

            iterator& operator++() noexcept
            {
                _iter._advance = true;
                return *this;
            }

            bool operator!=(const sentinel&) const noexcept
            {
                return _iter.valid();
            }

        private:
            split_iterator& _iter;
        };

        explicit constexpr split_iterator(const std::basic_string_view<T, Traits>& str, T needle) noexcept :
            _it{ str.begin() },
            _end{ str.end() },
            _needle{ needle }
        {
        }

        iterator begin() noexcept
        {
            return iterator{ *this };
        }

        sentinel end() noexcept
        {
            return sentinel{};
        }

    private:
        bool valid() const noexcept
        {
            return !_endReached;
        }

        iterator::reference value() noexcept
        {
            if (_advance)
            {
                const auto it = std::find(_it, _end, _needle);
                _value = { _it, it };
                _endReached = it == _end;
                _it = it + (_endReached ? 0 : 1);
            }
            return _value;
        }

        iterator::value_type::iterator _it;
        iterator::value_type::iterator _end;
        iterator::value_type _value;
        T _needle;
        bool _advance = true;
        bool _endReached = false;
    };

    template<typename T, typename Traits>
    constexpr std::basic_string_view<T, Traits> trim(const std::basic_string_view<T, Traits>& str, const T ch) noexcept
    {
        auto beg = str.data();
        auto end = beg + str.size();

        for (; beg != end && *beg == ch; ++beg)
        {
        }

        for (; beg != end && end[-1] == ch; --end)
        {
        }

        return { beg, end };
    }

    // Splits a font-family list into individual font-families. It loosely follows the CSS spec for font-family.
    // It splits by comma, handles quotes and simple escape characters, and it cleans whitespace.
    //
    // This is not the right place to put this, because it's highly specialized towards font-family names.
    // But this code is needed both, in our renderer and in our settings UI. At the time I couldn't find a better place for it.
    void iterate_font_families(const std::wstring_view& families, auto&& callback)
    {
        std::wstring family;
        bool escape = false;
        bool delayedSpace = false;
        wchar_t stringType = 0;

        for (const auto ch : families)
        {
            if (!escape)
            {
                switch (ch)
                {
                case ' ':
                    if (stringType)
                    {
                        // Spaces are treated literally inside strings.
                        break;
                    }
                    delayedSpace = !family.empty();
                    continue;
                case '"':
                case '\'':
                    if (stringType && stringType != ch)
                    {
                        // Single quotes inside double quotes are treated literally and vice versa.
                        break;
                    }
                    stringType = stringType == ch ? 0 : ch;
                    continue;
                case ',':
                    if (stringType)
                    {
                        // Commas are treated literally inside strings.
                        break;
                    }
                    if (!family.empty())
                    {
                        callback(std::move(family));
                        family.clear();
                        delayedSpace = false;
                    }
                    continue;
                case '\\':
                    escape = true;
                    continue;
                default:
                    break;
                }
            }

            // The `delayedSpace` logic automatically takes care for us to
            // strip leading and trailing spaces and deduplicate them too.
            if (delayedSpace)
            {
                delayedSpace = false;
                family.push_back(L' ');
            }

            family.push_back(ch);
            escape = false;
        }

        // Just like the comma handler above.
        if (!stringType && !family.empty())
        {
            callback(std::move(family));
        }
    }

    // This function is appropriate for case-insensitive equivalence testing of file paths and other "system" strings.
    // Similar to memcmp, this returns <0, 0 or >0.
    inline int compare_ordinal_insensitive(const std::wstring_view& lhs, const std::wstring_view& rhs) noexcept
    {
        const auto lhsLen = ::base::saturated_cast<int>(lhs.size());
        const auto rhsLen = ::base::saturated_cast<int>(rhs.size());
        // MSDN:
        // > To maintain the C runtime convention of comparing strings,
        // > the value 2 can be subtracted from a nonzero return value.
        // > [...]
        // > The function returns 0 if it does not succeed. [...] following error codes:
        // > * ERROR_INVALID_PARAMETER. Any of the parameter values was invalid.
        // -> We can just subtract 2.
        return CompareStringOrdinal(lhs.data(), lhsLen, rhs.data(), rhsLen, TRUE) - 2;
    }

    // This function is appropriate for sorting strings primarily used for human consumption, like a list of file names.
    // Similar to memcmp, this returns <0, 0 or >0.
    inline int compare_linguistic_insensitive(const std::wstring_view& lhs, const std::wstring_view& rhs) noexcept
    {
        const auto lhsLen = ::base::saturated_cast<int>(lhs.size());
        const auto rhsLen = ::base::saturated_cast<int>(rhs.size());
        // MSDN:
        // > To maintain the C runtime convention of comparing strings,
        // > the value 2 can be subtracted from a nonzero return value.
        // > [...]
        // > The function returns 0 if it does not succeed. [...] following error codes:
        // > * ERROR_INVALID_FLAGS. The values supplied for flags were invalid.
        // > * ERROR_INVALID_PARAMETER. Any of the parameter values was invalid.
        // -> We can just subtract 2.
#pragma warning(suppress : 26477) // Use 'nullptr' rather than 0 or NULL (es.47).
        return CompareStringEx(LOCALE_NAME_USER_DEFAULT, LINGUISTIC_IGNORECASE, lhs.data(), lhsLen, rhs.data(), rhsLen, nullptr, nullptr, 0) - 2;
    }

    // This function is appropriate for strings primarily used for human consumption, like a list of file names.
    inline bool contains_linguistic_insensitive(const std::wstring_view& str, const std::wstring_view& needle) noexcept
    {
        const auto strLen = ::base::saturated_cast<int>(str.size());
        const auto needleLen = ::base::saturated_cast<int>(needle.size());
        // MSDN:
        // > Returns a 0-based index into the source string indicated by lpStringSource if successful.
        // > [...]
        // > The function returns -1 if it does not succeed.
        // > * ERROR_INVALID_FLAGS. The values supplied for flags were not valid.
        // > * ERROR_INVALID_PARAMETER. Any of the parameter values was invalid.
        // > * ERROR_SUCCESS. The action completed successfully but yielded no results.
        // -> We can just check for -1.
#pragma warning(suppress : 26477) // Use 'nullptr' rather than 0 or NULL (es.47).
        return FindNLSStringEx(LOCALE_NAME_USER_DEFAULT, LINGUISTIC_IGNORECASE, str.data(), strLen, needle.data(), needleLen, nullptr, nullptr, nullptr, 0) != -1;
    }
}
