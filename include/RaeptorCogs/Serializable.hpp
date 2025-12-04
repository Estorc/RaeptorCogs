/** ********************************************************************************
 * @section Serializable_Overview Overview
 * @file Serializable.hpp
 * @brief Serialization utilities.
 * @details
 * Typical use cases:
 * - Serializing and deserializing objects to/from binary streams
 * - Reflecting object fields for custom processing
 * *********************************************************************************
 * @section Serializable_Header Header
 * <RaeptorCogs/Serializable.hpp>
 ***********************************************************************************
 * @section Serializable_Metadata Metadata
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
#include <RaeptorCogs/Flags.hpp>
#include <vector>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <variant>
#include <glm/matrix.hpp>

namespace RaeptorCogs {

class Visitor;


/**
 * @brief Serialization flags.
 * 
 * Defines flags for serialization options.
 */
enum class SerializationFlag : uint8_t {
    /** No serialization flags set. */
    NONE = 0,
    /** Enable compression for serialized data. */
    COMPRESS = 1 << 0,
};

/**
 * @brief Serializable interface.
 * 
 * Provides a contract for serializable objects.
 */
class Serializable {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~Serializable() = default;

        /**
         * @brief Reflect method to be implemented by derived classes.
         * 
         * @param v Visitor to operate on the fields.
         */
        virtual void reflect(Visitor& v) = 0;

        /**
         * @brief Dump the object to an output stream.
         * 
         * @param flags Serialization flags to control the dumping process.
         * @return Vector of bytes representing the serialized object.
         */
        std::vector<uint8_t> dump(SerializationFlag flags = SerializationFlag::NONE);

        /**
         * @brief Load the object from an input stream.
         * 
         * @param data Vector of bytes representing the serialized object.
         * @param flags Serialization flags to control the loading process.
         */
        void load(std::vector<uint8_t> data, SerializationFlag flags = SerializationFlag::NONE);
};


/**
 * @brief Visitor mode enumeration.
 * 
 * Defines the mode of operation for the visitor (serialization or deserialization).
 */
enum class VisitorMode : uint8_t {
    /** @brief Serialization mode. */
    SERIALIZE,
    /** @brief Deserialization mode. */
    DESERIALIZE
};

/**
 * @brief Visitor interface.
 * 
 * Defines the interface for visitors that can operate on different field types.
 */
class Visitor {
    private:

        // ============================================================================
        //                             PRIVATE MEMBERS
        // ============================================================================

        /**
         * @brief Stream variant for input/output operations.
         */
        std::iostream& stream;

        /**
         * @brief Current mode of the visitor (serialize or deserialize).
         */
        VisitorMode mode;
    public:

        // ============================================================================
        //                             PUBLIC METHODS
        // ============================================================================


        /** 
         * @brief Visit a Serializable object field.
         */
        template<typename T>
        std::enable_if_t<std::is_base_of_v<Serializable, T>> 
        visit(T& obj) {
            obj.reflect(*this);
        }

        /**
         * @brief Visit a generic field.
         */
        template<typename T>
        std::enable_if_t<!std::is_base_of_v<Serializable, T>>
        visit(T& v) {
            switch (mode) {
                case VisitorMode::SERIALIZE:
                    write(v);
                    break;
                case VisitorMode::DESERIALIZE:
                    read(v);
                    break;
            }
        }

        /**
         * @brief Read a value from the stream.
         */
        template<typename T>
        void read(T& v) {
            stream.read(reinterpret_cast<char*>(&v), sizeof(T));
        }

        /**
         * @brief Read a string from the stream.
         */
        void read(std::string& str) {
            uint32_t size;
            stream.read(reinterpret_cast<char*>(&size), sizeof(size));
            str.resize(size);
            stream.read(&str[0], size);
        }
        
        /** 
         * @brief Read a vector from the stream.
         */
        template<typename T>
        void read(std::vector<T>& vec) {
            uint32_t size;
            stream.read(reinterpret_cast<char*>(&size), sizeof(size));
            vec.resize(size);
            for (uint32_t i = 0; i < size; ++i) {
                read(vec[i]);
            }
        }

        /**
         * @brief Write a value to the stream.
         */
        template<typename T>
        void write(T& v) {
            stream.write(reinterpret_cast<const char*>(&v), sizeof(T));
        }

        /**
         * @brief Write a string to the stream.
         */
        void write(std::string& str) {
            uint32_t size = (uint32_t)str.size();
            stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
            stream.write(str.data(), size);
        }

        /** 
         * @brief Write a vector to the stream.
         */
        template<typename T>
        void write(std::vector<T>& vec) {
            uint32_t size = (uint32_t)vec.size();
            stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
            for (uint32_t i = 0; i < size; ++i) {
                write(vec[i]);
            }
        }

        /** 
         * @brief Constructor.
         * 
         * @param stream Stream to read from or write to.
         * @param mode Mode of the visitor (serialize or deserialize).
         */
        Visitor(std::iostream& stream, VisitorMode mode) : stream(stream), mode(mode) {}

        /** @brief Destructor. */
        ~Visitor() = default;
};

} // namespace RaeptorCogs