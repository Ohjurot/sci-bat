/*!
 * @file Exception.h
 * @brief Implementation ontop of std::exception.
 * @author Ludwig Fuechsl <ludwig.fuechsl@hm.edu>
 */
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
    /*!
     * @brief Custom (formated) exception.
    */
    class Exception : public std::exception
    {
        public:
            Exception() = delete;
            /*!
             * @brief Create a plain text based exception.
             * @param text Exception message.
            */
            Exception(const std::string_view& text) :
                m_text(text)
            {}

            /*!
             * @brief Create a text exception using a format string and argument(s).
             * @tparam ...Args Type of arguments. Should be deduced automatically.
             * @param f Format string.
             * @param ...args Argument value(s).
            */
            template<typename... Args>
            Exception(const std::string_view& f, Args... args)
            {
                m_text = ::fmt::vformat(f, fmt::make_format_args(std::forward<Args>(args)...));
            }

            Exception(const Exception&) = default;
            Exception(Exception&&) noexcept = default;

            Exception& operator=(const Exception&) = default;
            Exception& operator=(Exception&&) noexcept = default;

            /*!
             * @brief Retrive the message of the exception.
             * @return Exception message as plain c string. 
            */
            char const* what() const noexcept override
            {
                return m_text.c_str();
            }

        private:
            std::string m_text;
    };
}
