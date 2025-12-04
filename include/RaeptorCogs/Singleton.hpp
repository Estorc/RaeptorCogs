/** ********************************************************************************
 * @section Singleton_Overview Overview
 * @file Singleton.hpp
 * @brief High-level singleton handling utilities.
 * @details
 * Typical use cases:
 * - Managing singleton instances of classes.
 * *********************************************************************************
 * @section Singleton_Header Header
 * <RaeptorCogs/Singleton.hpp>
 ***********************************************************************************
 * @section Singleton_Metadata Metadata
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

/**
 * @namespace RaeptorCogs::Singletons
 * 
 * @brief Namespace for singleton utilities.
 * 
 * Provides functionalities to manage singleton instances of classes.
 * @see RaeptorCogs::SingletonAccessor
 */
namespace RaeptorCogs::Singletons {}

namespace RaeptorCogs {
    /**
     * @brief SingletonAccessor struct.
     * 
     * Provides access to singleton instances of classes.
     * 
     * @tparam T The class type to manage as a singleton.
     * 
     * @code{.cpp}
     * class MySingleton {
     * private:
     *      MySingleton() = default; // Prevent direct instantiation
     * public:
     *      void doSomething() {}
     *      friend RaeptorCogs::SingletonAccessor<MySingleton>;
     * };
     * // Access the singleton instance
     * MySingleton& instance = RaeptorCogs::SingletonAccessor<MySingleton>::get();
     * instance.doSomething();
     * @endcode
     */
    template <typename T>
    struct SingletonAccessor {
        /**
         * @brief Get the singleton instance of the class T.
         * 
         * @return Reference to the singleton instance of T.
         * 
         * @note The constructor of T must be private or protected, and
         *       SingletonAccessor must be declared as a friend of T. Accessing
         *       this method will create the instance on first call and return
         *       a reference to it.
         */
        static T& get() {
            static T inst;
            return inst;
        }
    };
}