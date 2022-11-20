#pragma once

#include <type_traits>
#include <thread>

namespace SCI::Util
{
    class ILock
    {
        public:
            virtual ~ILock() = default;
            
            virtual bool TryAquire() = 0;
            virtual void Release() = 0;

            template<typename T = void(*)(void), typename = std::enable_if_t<std::is_invocable_v<T>>>
            void Aquire(T f = std::this_thread::yield)
            {
                while (!TryAquire())
                    f();
            }

            template<typename Arg, typename T = void(*)(Arg), typename = std::enable_if_t<std::is_invocable_v<T, Arg>>>
            void Aquire(T f, Arg arg)
            {
                while (!TryAquire())
                    f(arg);
            }

            template<typename... Args, typename T = void(*)(Args...), typename = std::enable_if_t<std::is_invocable_v<T, Args...>>>
            void Aquire(T f, Args... args)
            {
                while (!TryAquire())
                    f(args...);
            }
    };
}
