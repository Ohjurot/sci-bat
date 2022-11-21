#pragma once

#include <fmt/format.h>

#include <exception>
#include <string>
#include <string_view>

#define SCI_EXCEPTION(text) ::SCI::Util::Exception(text)
#define SCI_EXCEPTION_FMT(text, ...) ::SCI::Util::Exception(text, __VA_ARGS__)
#define SCI_ASSERT(expr, text) if(!(expr)) { throw SCI_EXCEPTION(text); }
#define SCI_ASSERT_FMT(expr, text, ...) if(!(expr)) { throw SCI_EXCEPTION_FMT(text, __VA_ARGS__); }

namespace SCI::Util
{
    class Exception : public std::exception
    {
        public:
            Exception() = delete;
            Exception(const std::string_view& text) :
                m_text(text)
            {}
            template<typename Arg>
            Exception(const std::string_view& f, Arg arg)
            {
                m_text = ::fmt::vformat(f, fmt::make_format_args(std::forward<Arg>(arg)));
            }
            template<typename... Args>
            Exception(const std::string_view& f, Args... args)
            {
                m_text = ::fmt::vformat(f, fmt::make_format_args(std::forward<Args>(args)...));
            }

            Exception(const Exception&) = default;
            Exception(Exception&&) noexcept = default;

            Exception& operator=(const Exception&) = default;
            Exception& operator=(Exception&&) noexcept = default;

            char const* what() const override
            {
                return m_text.c_str();
            }

        private:
            std::string m_text;
    };
}
