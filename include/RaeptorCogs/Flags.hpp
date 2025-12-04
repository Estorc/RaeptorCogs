/** ********************************************************************************
 * @section Flags_Overview Overview
 * @file Flags.hpp
 * @brief Utilities for enabling bitmask operations on enum classes.
 * @details
 * Typical use cases:
 * - Enabling bitmask operations on enum classes
 * *********************************************************************************
 * @section Flags_Header Header
 * <RaeptorCogs/Flags.hpp>
 ***********************************************************************************
 * @section Flags_Metadata Metadata
 * @author Estorc
 * @version v1.0
 * @copyright Copyright (c) 2025 Estorc MIT License.
 **********************************************************************************/
/*                             This file is part of
 *                                  RaeptorCogs
 *                     (https://github.com/Estorc/RaeptorCogs)
 ***********************************************************************************
 * Copyright (c) 2025 Estorc.
 * This file is licensed under the MIT License.
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ***********************************************************************************/

#pragma once
#include <type_traits>
#include <tuple>
#include <cstdint>

namespace RaeptorCogs {

/**
 * @brief Check if a enum is a class enum.
 */
template <class T>
struct isClassEnum {
    static constexpr bool value =
        std::is_enum<T>::value &&
        !std::is_convertible<T, int>::value;
};

/**
 * @brief Check if a enum is a plain enum.
 */
template <class T>
struct isPlainEnum {
    static constexpr bool value =
        std::is_enum<T>::value &&
         std::is_convertible<T, int>::value;
};

/**
 * @brief Enable OR bitmask operator for a given enum class.
 * 
 * @param a First operand.
 * @param b Second operand.
 * @return The result of the bitwise OR operation.
 * 
 * @code{.cpp}
 * enum class MyFlags : uint32_t {
 *  FLAG_A = 1 << 0, // 01
 *  FLAG_B = 1 << 1, // 10
 * };
 * MyFlags union = MyFlags::FLAG_A | MyFlags::FLAG_B; // 11
 * @endcode
 */
template<typename Enum>
constexpr typename std::enable_if_t<isClassEnum<Enum>::value, Enum>
operator|(Enum a, Enum b) {
    using Under = std::underlying_type_t<Enum>;
    return static_cast<Enum>(static_cast<Under>(a) | static_cast<Under>(b));
}

/**
 * @brief Enable AND bitmask operator for a given enum class.
 * 
 * @param a First operand.
 * @param b Second operand.
 * @return The result of the bitwise AND operation.
 * 
 * @code{.cpp}
 * enum class MyFlags : uint32_t {
 *  FLAG_A = 1 << 0, // 01
 *  FLAG_B = 1 << 1, // 10
 * };
 * MyFlags intersection = MyFlags::FLAG_A & MyFlags::FLAG_B; // 00
 * @endcode
 */
template<typename Enum>
constexpr typename std::enable_if_t<isClassEnum<Enum>::value, Enum>
operator&(Enum a, Enum b) {
    using Under = std::underlying_type_t<Enum>;
    return static_cast<Enum>(static_cast<Under>(a) & static_cast<Under>(b));
}

/**
 * @brief Enable XOR bitmask operator for a given enum class.
 * 
 * @param a First operand.
 * @param b Second operand.
 * @return The result of the bitwise XOR operation.
 * 
 * @code{.cpp}
 * enum class MyFlags : uint32_t {
 *  FLAG_A = 1 << 0, // 01
 *  FLAG_B = 1 << 1, // 10
 * };
 * MyFlags xorFlags = MyFlags::FLAG_A ^ MyFlags::FLAG_B; // 11
 * @endcode
 */
template<typename Enum>
constexpr typename std::enable_if_t<isClassEnum<Enum>::value, Enum>
operator^(Enum a, Enum b) {
    using Under = std::underlying_type_t<Enum>;
    return static_cast<Enum>(static_cast<Under>(a) ^ static_cast<Under>(b));
}

/**
 * @brief Enable NOT bitmask operator for a given enum class.
 * 
 * @param a The enum class value.
 * @return The result of the bitwise NOT operation.
 * 
 * @code{.cpp}
 * enum class MyFlags : uint32_t {
 *  FLAG_A = 1 << 0, // 01
 *  FLAG_B = 1 << 1, // 10
 * };
 * MyFlags notA = ~MyFlags::FLAG_A; // 10
 * @endcode
 */
template<typename Enum>
constexpr typename std::enable_if_t<isClassEnum<Enum>::value, Enum>
operator~(Enum a) {
    using Under = std::underlying_type_t<Enum>;
    return static_cast<Enum>(~static_cast<Under>(a));
}

/**
 * @brief Enable OR assignment bitmask operator for a given enum class.
 *
 * @param a Variable to modify.
 * @param b Value to OR with.
 * @return Reference to the modified enum after the operation.
 * 
 * @code{.cpp}
 * enum class MyFlags : uint32_t {
 *  FLAG_A = 1 << 0,
 *  FLAG_B = 1 << 1,
 * };
 * MyFlags flags = MyFlags::FLAG_A;
 * flags |= MyFlags::FLAG_B; // flags now has both FLAG_A and FLAG_B set
 * @endcode
 */
template<typename Enum>
constexpr typename std::enable_if_t<isClassEnum<Enum>::value, Enum&>
operator|=(Enum& a, Enum b) {
    return a = a | b;
}

/**
 * @brief Enable AND assignment bitmask operator for a given enum class.
 *
 * @param a Variable to modify.
 * @param b Value to AND with.
 * @return Reference to the modified enum after the operation.
 * 
 * @code{.cpp}
 * enum class MyFlags : uint32_t {
 *  FLAG_A = 1 << 0,
 *  FLAG_B = 1 << 1,
 * };
 * MyFlags flags = MyFlags::FLAG_A | MyFlags::FLAG_B;
 * flags &= MyFlags::FLAG_A; // flags now only has FLAG_A set
 * @endcode
 */
template<typename Enum>
constexpr typename std::enable_if_t<isClassEnum<Enum>::value, Enum&>
operator&=(Enum& a, Enum b) {
    return a = a & b;
}

/**
 * @brief Enable XOR assignment bitmask operator for a given enum class.
 * 
 * @param a Variable to modify.
 * @param b Value to XOR with.
 * @return Reference to the modified enum after the operation.
 * 
 * @code{.cpp}
 * enum class MyFlags : uint32_t {
 *  FLAG_A = 1 << 0,
 *  FLAG_B = 1 << 1,
 * };
 * MyFlags flags = MyFlags::FLAG_A;
 * flags ^= MyFlags::FLAG_B; // flags now has both FLAG_A and FLAG_B set
 * @endcode
 */
template<typename Enum>
constexpr typename std::enable_if_t<isClassEnum<Enum>::value, Enum&>
operator^=(Enum& a, Enum b) {
    return a = a ^ b;
}

/**
 * @brief Enable NOT operator for a given enum class.
 * 
 * @param a The enum class value.
 * @return true if the enum value is zero, false otherwise.
 * 
 * @code{.cpp}
 * enum class MyFlags : uint32_t {
 *  FLAG_A = 1 << 0,
 *  FLAG_B = 1 << 1,
 * };
 * MyFlags flags = MyFlags::NONE;
 * bool isNone = !flags; // true if no flags are set
 * @endcode
 */
template<typename Enum>
constexpr typename std::enable_if_t<isClassEnum<Enum>::value, bool>
operator!(Enum a) {
    using Under = std::underlying_type_t<Enum>;
    return static_cast<Under>(a) == 0;
}

/**
 * @brief Enable equality operator for a given enum class.
 * 
 * @param a First operand.
 * @param b Second operand.
 * @return true if both enum values are equal, false otherwise.
 * 
 * @code{.cpp}
 * enum class MyFlags : uint32_t {
 *  FLAG_A = 1 << 0,
 *  FLAG_B = 1 << 1,
 * };
 * bool isEqual = (MyFlags::FLAG_A == MyFlags::FLAG_A); // true
 * @endcode
 */
template<typename Enum>
constexpr typename std::enable_if_t<isClassEnum<Enum>::value, bool>
operator==(Enum a, Enum b) {
    using Under = std::underlying_type_t<Enum>;
    return static_cast<Under>(a) == static_cast<Under>(b);
}

/**
 * @brief Enable inequality operator for a given enum class.
 * 
 * @param a First operand.
 * @param b Second operand.
 * @return true if both enum values are not equal, false otherwise.
 * 
 * @code{.cpp}
 * enum class MyFlags : uint32_t {
 *  FLAG_A = 1 << 0,
 *  FLAG_B = 1 << 1,
 * };
 * bool isNotEqual = (MyFlags::FLAG_A != MyFlags::FLAG_B); // true
 * @endcode
 */
template<typename Enum>
constexpr typename std::enable_if_t<isClassEnum<Enum>::value, bool>
operator!=(Enum a, Enum b) {
    using Under = std::underlying_type_t<Enum>;
    return static_cast<Under>(a) != static_cast<Under>(b);
}

/**
 * @brief Storage structure for enum flags.
 */
template<typename Enum>
struct EnumStorage {
    using T = std::underlying_type_t<Enum>;
    T bits = 0;
};

/**
 * @brief FlagSet class for managing multiple enum class flags.
 * 
 * This class allows you to manage flags from multiple enum classes in a single structure.
 * 
 * @code{.cpp}
 * enum class FlagEnumA : uint32_t {
 *  FLAG_A1 = 1 << 0,
 *  FLAG_A2 = 1 << 1,
 * };
 * enum class FlagEnumB : uint32_t {
 *  FLAG_B1 = 1 << 0,
 *  FLAG_B2 = 1 << 1,
 * };
 * 
 * RaeptorCogs::FlagSet<FlagEnumA, FlagEnumB> flagSet;
 * flagSet.setFlag(FlagEnumA::FLAG_A1);
 * flagSet.setFlag(FlagEnumB::FLAG_B2);
 * bool hasA1 = flagSet.hasFlag(FlagEnumA::FLAG_A1); // true
 * bool hasB1 = flagSet.hasFlag(FlagEnumB::FLAG_B1); // false
 * @endcode
 */
template<typename... Enums>
class FlagSet {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Stored flags.
         * 
         * Holds the combined flags for all enum types.
         */
        std::tuple<EnumStorage<Enums>...> storage;

        /**
         * @brief Get the storage for a specific enum type.
         * 
         * @param t The enum class type.
         * @return Reference to the storage for the specified enum type.
         */
        template<typename Enum>
        static constexpr auto& getStorage(std::tuple<EnumStorage<Enums>...>& t) {
            return std::get<EnumStorage<Enum>>(t);
        }

        /**
         * @brief Get the storage for a specific enum type (const version).
         * 
         * @param t The enum class type.
         * @return Const reference to the storage for the specified enum type.
         */
        template<typename Enum>
        static constexpr const auto& getStorage(const std::tuple<EnumStorage<Enums>...>& t) {
            return std::get<EnumStorage<Enum>>(t);
        }

    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Default constructor for FlagSet.
         */
        FlagSet() = default;

        /**
         * @brief Destructor for FlagSet.
         */
        ~FlagSet() = default;

        /**
         * @brief Set a flag.
         * 
         * @param flag Flag to set.
         */
        template<typename Enum>
        void setFlag(Enum flag) {
            static_assert((std::is_same_v<Enum, Enums> || ...),
                        "FlagSet: Enum type not registered");
            getStorage<Enum>(storage).bits |= static_cast<std::underlying_type_t<Enum>>(flag);
        }

        /**
         * @brief Toggle a flag.
         * 
         * @param flag Flag to toggle.
         */
        template<typename Enum>
        void toggleFlag(Enum flag) {
            static_assert((std::is_same_v<Enum, Enums> || ...),
                        "FlagSet: Enum type not registered");
            getStorage<Enum>(storage).bits ^= static_cast<std::underlying_type_t<Enum>>(flag);
        }

        /**
         * @brief Clear a flag.
         * 
         * @param flag Flag to clear.
         */
        template<typename Enum>
        void clearFlag(Enum flag) {
            getStorage<Enum>(storage).bits &= ~static_cast<std::underlying_type_t<Enum>>(flag);
        }

        /**
         * @brief Clear all flags.
         */
        void clearFlag() {
            (..., (getStorage<Enums>(storage).bits = 0));
        }

        /**
         * @brief Check if a flag is set.
         * 
         * @param flag Flag to check.
         */
        template<typename Enum>
        bool hasFlag(Enum flag) const {
            return (getStorage<Enum>(storage).bits &
                    static_cast<std::underlying_type_t<Enum>>(flag)) != 0;
        }
};

}