#pragma once
#include <type_traits>

namespace RaeptorCogs {
template<typename Enum>
struct EnableBitmaskOperators {
    static constexpr bool enable = false;
};

template<typename Enum>
constexpr bool EnableBitmaskOperators_v = EnableBitmaskOperators<Enum>::enable;

// Define operators only for enums with EnableBitmaskOperators<Enum>::enable == true

template<typename Enum>
constexpr typename std::enable_if_t<EnableBitmaskOperators_v<Enum>, Enum>
operator|(Enum a, Enum b) {
    using Under = std::underlying_type_t<Enum>;
    return static_cast<Enum>(static_cast<Under>(a) | static_cast<Under>(b));
}

template<typename Enum>
constexpr typename std::enable_if_t<EnableBitmaskOperators_v<Enum>, Enum>
operator&(Enum a, Enum b) {
    using Under = std::underlying_type_t<Enum>;
    return static_cast<Enum>(static_cast<Under>(a) & static_cast<Under>(b));
}

template<typename Enum>
constexpr typename std::enable_if_t<EnableBitmaskOperators_v<Enum>, Enum>
operator~(Enum a) {
    using Under = std::underlying_type_t<Enum>;
    return static_cast<Enum>(~static_cast<Under>(a));
}

template<typename Enum>
constexpr typename std::enable_if_t<EnableBitmaskOperators_v<Enum>, Enum&>
operator|=(Enum& a, Enum b) {
    return a = a | b;
}

template<typename Enum>
constexpr typename std::enable_if_t<EnableBitmaskOperators_v<Enum>, Enum&>
operator&=(Enum& a, Enum b) {
    return a = a & b;
}

template<typename Enum>
constexpr typename std::enable_if_t<EnableBitmaskOperators_v<Enum>, bool>
operator!(Enum a) {
    using Under = std::underlying_type_t<Enum>;
    return static_cast<Under>(a) == 0;
}

template<typename Enum>
constexpr typename std::enable_if_t<EnableBitmaskOperators_v<Enum>, bool>
operator==(Enum a, Enum b) {
    using Under = std::underlying_type_t<Enum>;
    return static_cast<Under>(a) == static_cast<Under>(b);
}

template<typename Enum>
constexpr typename std::enable_if_t<EnableBitmaskOperators_v<Enum>, bool>
operator!=(Enum a, Enum b) {
    using Under = std::underlying_type_t<Enum>;
    return static_cast<Under>(a) != static_cast<Under>(b);
}

template <typename Enum>
constexpr typename std::enable_if_t<EnableBitmaskOperators_v<Enum>, bool>
any(Enum flags) {
    using Under = std::underlying_type_t<Enum>;
    return static_cast<Under>(flags) != 0;
}
}