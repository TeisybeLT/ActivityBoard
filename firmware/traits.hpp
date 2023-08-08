#ifndef TRAITS_HPP
#define TRAITS_HPP

// Implementation of these traits and concepts are basically stolen from cppreference.com

namespace traits
{
    template<typename Type, Type val>
    struct integral_constant
    {
        static constexpr Type value = val;
    
        using value_type = Type;
        using type = integral_constant;

        constexpr operator value_type() const noexcept
        {
            return value;
        }

        constexpr value_type operator()() const noexcept
        {
            return value;
        }
    };
    
    template<bool B>
    using bool_constant = integral_constant<bool, B>;
    using true_type = integral_constant<bool, true>;
    using false_type = integral_constant<bool, false>;
    
    template<typename T>
    struct is_integral : bool_constant<
        requires (T t, T* p, void (*f)(T))
        {
            reinterpret_cast<T>(t);
            f(0);
            p + t;
        }
    > {}; 
    
    template<typename T>
    inline constexpr auto is_integral_v = is_integral<T>::value;

    namespace detail
    {
        template<typename T, bool = is_integral<T>::value>
        struct is_signed : integral_constant<bool, T(-1) < T(0)>
        {};
 
        template<typename T>
        struct is_signed<T, false> : false_type
        {};
    }
 
    template<typename T>
    struct is_signed : detail::is_signed<T>::type
    {};
    
    template<typename T>
    inline constexpr auto is_signed_v = is_signed<T>::value;
    
    template<typename T>
    concept integral = is_integral_v<T>;

    template<typename T>
    concept signed_integral = integral<T> && is_signed_v<T>;

    template<typename T>
    concept unsigned_integral = integral<T> && !signed_integral<T>;
}

#endif