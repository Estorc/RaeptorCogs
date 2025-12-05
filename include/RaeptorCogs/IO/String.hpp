/** ********************************************************************************
 * @section IO_String_Overview Overview
 * @file String.hpp
 * @brief High-level UTF-8 string handling utilities.
 * @details
 * Typical use cases:
 * - Handling UTF-8 encoded strings and characters
 * *********************************************************************************
 * @section IO_String_Header Header
 * <RaeptorCogs/IO/String.hpp>
 ***********************************************************************************
 * @section IO_String_Metadata Metadata
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
#include <string>
namespace RaeptorCogs {


class U8CharIterator;

/**
 * @brief UTF-8 Character class.
 * 
 * Represents a single UTF-8 encoded character and provides utilities for handling it.
 * 
 * @code{.cpp}
 * RaeptorCogs::U8Char ch = RaeptorCogs::U8Char("é");
 * uint32_t codepoint = ch.codepoint(); // U+00E9
 * @endcode
 */
class U8Char {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Underlying string view representing the UTF-8 character.
         * 
         * Stores the bytes of the UTF-8 character.
         * 
         * @note The size of the view can be between 1 and 4 bytes depending on the character.
         */
        std::string_view view_;

        /**
         * @brief Determine the length of a UTF-8 character based on its first byte.
         * 
         * @param c The first byte of the UTF-8 character.
         * @return The length of the UTF-8 character in bytes (1 to 4).
         */
        static size_t utf8_len(unsigned char c) {
            if (c < 0x80) return 1;
            if ((c >> 5) == 0x6) return 2;
            if ((c >> 4) == 0xE) return 3;
            if ((c >> 3) == 0x1E) return 4;
            return 1; // invalid → treat as 1 byte to stay safe
        }

        friend class U8CharIterator;
        friend class U8String;
    public:
        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Construct a U8Char from a string view.
         * @param v A string view representing the UTF-8 character.
         */
        U8Char(std::string_view v) : view_(v.data(), utf8_len((unsigned char)v[0])) {}

        /**
         * @brief Construct a U8Char from a standard string.
         * @param s A standard string representing the UTF-8 character.
         */
        U8Char(const std::string& s) : U8Char(std::string_view(s.data(), utf8_len((unsigned char)s[0]))) {}

        /**
         * @brief Construct a U8Char from a C-style string and length.
         * @param s A pointer to the C-style string.
         * @param len The length of the UTF-8 character in bytes.
         */
        U8Char(const char* s, size_t len) : view_(s, len) {}

        /**
         * @brief Construct a U8Char from a C-style string.
         * @param s A pointer to the C-style string.
         */
        U8Char(const char* s) : view_(s, utf8_len((unsigned char)s[0])) {}

        /**
         * @brief Default constructor for U8Char.
         * Creates an empty U8Char.
         */
        U8Char() : view_("") {}

        /**
         * @brief Constructor for null U8Char.
         * Creates an empty U8Char.
         */
        U8Char(std::nullptr_t) : view_("") {}

        /**
         * @brief Conversion operator to std::string_view.
         * Allows implicit conversion to string view.
         */
        operator std::string_view() const noexcept { return view_; }
        bool operator==(std::string_view other) const noexcept {
            return view_ == other;
        }

        /**
         * @brief Get the size of the UTF-8 character in bytes.
         * @return The size of the UTF-8 character.
         */
        size_t size() const noexcept { return view_.size(); }

        /**
         * @brief Get the Unicode codepoint of the UTF-8 character.
         * @return The Unicode codepoint as a uint32_t.
         */
        uint32_t codepoint() const noexcept {
            if (view_.data()[0] == '\0') return 0xFFFDu;
            for (size_t i = 1; i < view_.size(); ++i) {
                if ((view_.data()[i] & 0xC0) != 0x80) {
                    return 0xFFFDu; // invalid continuation byte
                }
            }
            const unsigned char* s = (const unsigned char*)view_.data();
            switch (view_.size()) {
                case 1:
                    return s[0];
                case 2:
                    return ((s[0] & 0x1Fu) << 6u) |
                        (s[1] & 0x3Fu);
                case 3:
                    return ((s[0] & 0x0Fu) << 12u) |
                        ((s[1] & 0x3Fu) << 6u) |
                        (s[2] & 0x3Fu);
                case 4:
                    return ((s[0] & 0x07u) << 18u) |
                        ((s[1] & 0x3Fu) << 12u) |
                        ((s[2] & 0x3Fu) << 6u) |
                        (s[3] & 0x3Fu);
                default:
                    return 0xFFFDu; // replacement char
            }
        }

        /**
         * @brief Get the underlying string view of the UTF-8 character.
         * @return The string view representing the UTF-8 character.
         */
        std::string_view view() const noexcept { return view_; }
};

/**
 * @brief UTF-8 Character Iterator class.
 * Allows iteration over UTF-8 characters in a string.
 * @code{.cpp}
 * RaeptorCogs::U8String u8str("Hello, 世界");
 * for (const auto& ch : u8str) {
 *     std::cout << "Character: " << std::string(ch.view()) << ", Codepoint: U+" << std::hex << ch.codepoint() << std::dec << std::endl;
 * }
 * @endcode
 * 
 * @internal
 */
class U8CharIterator {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Pointer to the underlying string.
         */
        const std::string* data_;

        /**
         * @brief Current index in the string.
         */
        size_t index_;

    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /** @brief UTF-8 Character Iterator traits */
        using iterator_category = std::forward_iterator_tag;
        /** @brief The type of elements pointed to by the iterator */
        using value_type        = U8Char;
        /** @brief Difference type for iterator arithmetic */
        using difference_type   = std::ptrdiff_t;
        /** @brief Pointer type for the iterator */
        using pointer           = void;
        /** @brief Reference type for the iterator */
        using reference         = U8Char;

        /**
         * @brief Constructor for U8CharIterator.
         * 
         * @param s The string to iterate over.
         * @param idx The starting index for the iterator.
         * 
         * @note Used internally by U8String.
         */
        U8CharIterator(const std::string& s, size_t idx)
            : data_(&s), index_(idx) {}

        /**
         * @brief Dereference operator.
         * 
         * @return The current UTF-8 character.
         */
        reference operator*() const {
            unsigned char c = static_cast<unsigned char>((*data_)[index_]);
            size_t len = U8Char::utf8_len(c);
            return U8Char(data_->data() + index_, len);
        }

        /**
         * @brief Pre-increment operator.
         * 
         * @return Reference to the incremented iterator.
         */
        U8CharIterator& operator++() {
            unsigned char c = static_cast<unsigned char>((*data_)[index_]);
            index_ += U8Char::utf8_len(c);
            return *this;
        }

        /**
         * @brief Post-increment operator.
         * 
         * @return A copy of the iterator before incrementing.
         */
        U8CharIterator operator++(int) {
            U8CharIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        /**
         * @brief Addition operator.
         * 
         * @param n Number of characters to advance.
         * @return A new iterator advanced by n characters.
         */
        U8CharIterator operator+(int n) const {
            U8CharIterator tmp = *this;
            for (int i = 0; i < n; ++i) {
                ++tmp;
            }
            return tmp;
        }

        /**
         * @brief Equality operator.
         * 
         * @param o The other iterator to compare with.
         * @return true if both iterators are equal, false otherwise.
         */
        bool operator==(const U8CharIterator& o) const {
            return index_ == o.index_ && data_ == o.data_;
        }

        /**
         * @brief Inequality operator.
         * 
         * @param o The other iterator to compare with.
         * @return true if both iterators are not equal, false otherwise.
         */
        bool operator!=(const U8CharIterator& o) const {
            return !(*this == o);
        }
};

/**
 * @brief UTF-8 String class.
 * 
 * Represents a UTF-8 encoded string and provides utilities for handling it.
 * 
 * @code{.cpp}
 * RaeptorCogs::U8String u8str("Hello, 世界");
 * size_t length = u8str.size(); // Number of UTF-8 characters
 * U8Char ch = u8str[7]; // '世' character
 * @endcode
 */
class U8String {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Underlying standard string storing the UTF-8 data.
         * 
         * Stores the complete UTF-8 encoded string.
         * 
         * @note The string is managed internally and should not be modified directly.
         */
        std::string data;
    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Default constructor for U8String.
         */
        U8String() = default;

        /**
         * @brief Destructor for U8String.
         */
        ~U8String() = default;

        /**
         * @brief Copy constructor for U8String.
         * 
         * @param other The U8String to copy from.
         */
        U8String(const U8String& other) : data(other.data) {}

        /**
         * @brief Constructor from standard string.
         * 
         * @param other The standard string to construct from.
         */
        U8String(const std::string& other) : data(other) {}

        /**
         * @brief Move constructor from standard string.
         * 
         * @param other The standard string to move from.
         */
        U8String(std::string&& other) : data(std::move(other)) {}

        /**
         * @brief Constructor from C-style string.
         * 
         * @param s A pointer to the C-style string.
         */
        U8String(const char* s) : data(s) {}

        /**
         * @brief Constructor from U8Char.
         * 
         * @param ptr A pointer to the U8Char.
         */
        U8String(U8Char* ptr) : data(ptr->view().data()) {}

        /**
         * @brief Assignment operator for U8String.
         * 
         * @param other The U8String to assign from.
         */
        inline void operator=(const U8String& other) {
            this->data = other.data;
        }

        /**
         * @brief Equality operator.
         * 
         * @param other The U8String to compare with.
         * @return true if both U8Strings are equal, false otherwise.
         */
        bool operator==(const U8String& other) const {
            return data == other.data;
        }

        /**
         * @brief Conversion operator to C-style string.
         * @return Pointer to the C-style string.
         */
        const char *c_str() const {
            return data.c_str();
        }

        /**
         * @brief Get the first UTF-8 character of the string.
         * 
         * @param index The index of the UTF-8 character to access.
         * @return The U8Char at the specified index.
         */
        U8Char at(size_t index) const {
            U8CharIterator it(data, 0);
            U8CharIterator end(data, data.size());
            size_t current = 0;
            while (it != end) {
                if (current == index) {
                    return *it;
                }
                ++it;
                ++current;
            }
            return U8Char(); // Out of bounds
        }

        /**
         * @brief Subscript operator for U8String.
         * 
         * @param index The index of the UTF-8 character to access.
         * @return The U8Char at the specified index.
         */
        U8Char operator[](size_t index) const {
            return at(index);
        }

        /**
         * @brief Get the number of UTF-8 characters in the string.
         * 
         * @return The number of UTF-8 characters in the string.
         * @note This counts the actual UTF-8 characters, not bytes.
         */
        size_t size() const {
            size_t count = 0;
            U8CharIterator it(data, 0);
            U8CharIterator endIt(data, data.size());
            while (it != endIt) {
                ++count;
                ++it;
            }
            return count;
        }

        /**
         * @brief Begin iterator for U8String.
         * 
         * @return An iterator to the beginning of the UTF-8 characters.
         */
        auto begin() const {
            return U8CharIterator(data, 0);
        }
        
        /**
         * @brief End iterator for U8String.
         * 
         * @return An iterator to the end of the UTF-8 characters.
         */
        auto end() const {
            return U8CharIterator(data, data.size());
        }

        /**
         * @brief Begin iterator for U8String.
         * 
         * @return An iterator to the beginning of the UTF-8 characters.
         */
        auto begin() {
            return U8CharIterator(data, 0);
        }

        /**
         * @brief End iterator for U8String.
         * 
         * @return An iterator to the end of the UTF-8 characters.
         */
        auto end() {
            return U8CharIterator(data, data.size());
        }
};

}