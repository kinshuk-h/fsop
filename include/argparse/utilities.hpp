#ifndef UTILITIES_HPP_INCLUDED
#define UTILITIES_HPP_INCLUDED

#include <tuple>       // std::make_tuple, std::get
#include <type_traits> // std::is_disjunction_v

/**
 * @brief Generates Named Types for use in arguments of functions to better elaborate the context.
 *
 * @tparam ValueType The type being wrapped internally.
 * @tparam Descriptor A unique type to disambiguate the type from other types.
 */
template <typename ValueType, typename Descriptor>
class NamedType
{
    ValueType value_;
public:
    constexpr explicit NamedType(ValueType const& value) : value_(value) {}
    constexpr explicit NamedType(ValueType&& value)      : value_(std::move(value)) {}
    constexpr ValueType&       get()       { return value_; }
    constexpr const ValueType& get() const { return value_; }

    /**
     * @brief Type for representing variables usable for named arguments in functions.
     *
     * Variables of this type upon assignment returns a `NamedType` object, which allows for
     *  syntax similar to the following:
     *
     *  ```
     *      using Size = NamedType<int, struct SizeTag>;
     *      Size::Argument size;
     *      void init_window(Size size);
     *
     *      ...
     *
     *      init_window(size = 100);
     *  ```
     */
    struct Argument
    {
        Argument() = default;
        Argument(const Argument&) = delete;
        Argument(Argument&&)      = delete;

        Argument& operator=(const Argument&) = delete;
        Argument& operator=(Argument&&)      = delete;

        template<typename UnderlyingType>
        NamedType operator=(UnderlyingType&& value) const
        {
            return NamedType(std::forward<UnderlyingType>(value));
        }
    };
};

#if __cplusplus < 202000
    template< class T >
    struct remove_cvref {
        typedef std::remove_cv_t<std::remove_reference_t<T>> type;
    };
    template< class T >
    using remove_cvref_t = typename remove_cvref<T>::type;
#else
    template< class T >
    using remove_cvref_t = std::remove_cvref_t<T>;
#endif

/**
 * @brief Selects the first argument from a set of variadic arguments which matches a given strong type.
 *
 * @tparam TypeToPick The type of the variable to choose.
 * @tparam Types The Types to select from, usually deduced from the parameters.
 *
 * @param default_value The default value to use when no value could be selected.
 * @param args The arguments to choose from.
 *
 * @return {TypeToPick} The variable from the arguments whose type matches or the default value.
 */
template<typename TypeToPick, typename... Types>
constexpr TypeToPick pick_if(const TypeToPick& default_value, Types&&... args)
{
    if constexpr(std::disjunction_v<std::is_same<TypeToPick, remove_cvref_t<Types>>...>)
        return std::get<TypeToPick>(std::make_tuple(std::forward<Types>(args)...));
    return default_value;
}

#endif // UTILITIES_HPP_INCLUDED