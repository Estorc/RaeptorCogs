/** ********************************************************************************
 * @section GAPI_Common_Core_Object_Overview Overview
 * @file Object.hpp
 * @brief Object interface.
 * @details
 * Typical use cases:
 * - Defining a common interface for different graphics objects
 * *********************************************************************************
 * @section GAPI_Common_Core_Object_Header Header
 * <RaeptorCogs/Graphics/GAPI/Common/Resources/Object.hpp>
 ***********************************************************************************
 * @section GAPI_Common_Core_Object_Metadata Metadata
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
#include <typeindex>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <cstdint>
#include <vector>
#include <RaeptorCogs/Core/Region.hpp>

namespace RaeptorCogs::GAPI {
    class ObjectAllocator;
}

namespace RaeptorCogs::GAPI::Common {

class RendererBackend;

/**
 * @brief Object interface.
 * 
 * Provides a common interface for different graphics objects.
 * 
 * @note This is an abstract base class and should be inherited by specific object implementations.
 */
class ObjectData {
    private:
        /**
         * @brief Unique identifier for the object.
         * 
         * Used by the graphics API to reference the object.
         */
        size_t id = 0;

        /**
         * @brief Reference count for the object.
         * 
         * Manages the lifetime of the object.
         * 
         * @note Starts at 1 when allocated.
         */
        int refCount = 1;

        friend class RaeptorCogs::GAPI::ObjectAllocator;

    protected:

        // ============================================================================
        //                             PROTECTED MEMBERS
        // ============================================================================

        /**
         * @brief Default constructor.
         * 
         * Prevents direct instantiation of ObjectData.
         */
        ObjectData() = default;
    public:

        // ============================================================================
        //                             PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Virtual destructor.
         */
        virtual ~ObjectData() = default;


        /**
         * @brief Increment reference count.
         */
        void retain() {
            ++refCount;
        }

        /**
         * @brief Decrement reference count.
         * @return true if reference count reaches zero, false otherwise.
         * @note When reference count reaches zero, the object should be destroyed.
         */
        bool release() {
            --refCount;
            return refCount <= 0;
        }

        /**
         * @brief Bind the object for use.
         * 
         * Pure virtual method to be implemented by derived classes for binding the object.
         * 
         * @note Must be implemented by derived classes.
         */
        virtual void bind() = 0;

        /**
         * @brief Unbind the object.
         * 
         * Pure virtual method to be implemented by derived classes for unbinding the object.
         * 
         * @note Must be implemented by derived classes.
         */
        virtual void unbind() const = 0;

        /**
         * @brief Get the unique identifier of the object.
         * 
         * @return Unique identifier of the object.
         * 
         * @note Returns 0 if the object is not initialized.
         */
        virtual size_t getID() const { return this->id; }

        /**
         * @brief Check if the object is valid.
         * 
         * @return true if the object is valid, false otherwise.
         * 
         * @note Must be implemented by derived classes.
         */
        virtual bool isValid() const {
            std::cerr << "isValid method not implemented for " << typeid(*this).name() << "." << std::endl;
            return false;
        }
};

};


namespace RaeptorCogs::GAPI {
    /**
     * @brief Check if the renderer backend is initialized.
     * 
     * @return true if the renderer backend is initialized, false otherwise.
     * 
     * @note Used internally to verify renderer backend state.
     */
    bool IsRendererBackendInitialized();

    /**
     * @brief Object allocator static class.
     * 
     * Provides functionality to allocate object data instances.
     * 
     * @note Used internally for object data management.
     */
    class ObjectAllocator {
        private:
            /**
             * @brief Maximum number of objects.
             * 
             * Defines the maximum number of objects that can be allocated.
             */
            static constexpr size_t MAX_OBJECTS = SIZE_MAX;

            /**
             * @brief ID allocator for managing object IDs.
             * 
             * Used internally to allocate unique IDs for objects.
             */
            RegionAllocator idAllocator;

            /**
             * @brief Storage for allocated object data instances.
             * 
             * Holds shared pointers to allocated object data.
             * 
             * @note Used internally for object data management and destruction.
             */
            std::vector<RaeptorCogs::GAPI::Common::ObjectData*> allocatedObjects;
        public:
            /**
             * @brief Default constructor for ObjectAllocator.
             * 
             * Initializes the ObjectAllocator.
             */
            ObjectAllocator();

            /**
             * @brief Destructor for ObjectAllocator.
             * 
             * Cleans up allocated object data instances.
             */
            ~ObjectAllocator();

            /**
             * @brief Allocate object data of the specified type.
             * 
             * @param type Type index of the object data to retrieve.
             * @return Shared pointer to the object data.
             * 
             * @note Used internally to manage object data instances.
             */
            RaeptorCogs::GAPI::Common::ObjectData* allocate(std::type_index type);

            /**
             * @brief Destroy the specified object data instance.
             * 
             * @param object Pointer to the object data to destroy.
             * 
             * @note Used internally for object data management and cleanup.
             */
            void destroy(RaeptorCogs::GAPI::Common::ObjectData* object);

            /**
             * @brief Destroy the object data instance with the specified ID.
             * 
             * @param id Unique identifier of the object data to destroy.
             * 
             * @note Used internally for object data management and cleanup.
             */
            void destroy(size_t id);

            /**
             * @brief Destroy all allocated object data instances.
             * 
             * @note Used internally for cleanup purposes.
             */
            void destroyAll();
    };
    
    /**
     * @brief Allocate object data with the active renderer backend.
     * 
     * @param type Type index of the object data to allocate.
     * @return Pointer to the allocated object data.
     * 
     * @note Used internally to allocate object data using the active renderer backend.
     */
    RaeptorCogs::GAPI::Common::ObjectData* AllocateWithActiveBackend(std::type_index type);

    /**
     * @brief Free object data with the active renderer backend.
     * 
     * @param object Pointer to the object data to free.
     */
    void FreeWithActiveBackend(Common::ObjectData* object);

    /**
     * @brief Template class for handling graphics objects.
     * 
     * @tparam DataType Type of the object data, must derive from ObjectData.
     * 
     * @note Provides lazy initialization and access to the underlying object data.
     */
    template<typename DataType>
    class ObjectHandler {
        private:

            // ============================================================================
            //                             PRIVATE MEMBERS
            // ============================================================================

            /**
             * @brief Shared pointer to the object data.
             * 
             * Holds the instance of the object data.
             */
            DataType* data = nullptr;
        public:

            // ============================================================================
            //                             PUBLIC METHODS
            // ============================================================================

            /**
             * @brief Default constructor for ObjectHandler.
             */
            ObjectHandler() = default;

            /**
             * @brief Copy constructor for ObjectHandler.
             */
            ObjectHandler(const ObjectHandler& other) {
                if (other.data) other.data->retain();
                data = other.data;
            }

            /**
             * @brief Move constructor for ObjectHandler.
             */
            ObjectHandler(ObjectHandler&& other) noexcept {
                data = other.data;
                other.data = nullptr;
            }

            /**
             * @brief Copy assignment for ObjectHandler.
             */
            ObjectHandler& operator=(const ObjectHandler& other) {
                if (this != &other) {
                    if (other.data) other.data->retain();  // retain new first
                    if (data && data->release()) destroy(); // release old
                    data = other.data;
                }
                return *this;
            }

            /**
             * @brief Virtual destructor for ObjectHandler.
             */
            virtual ~ObjectHandler() {
                if (this->data && this->data->release()) this->destroy();
            }

            /**
             * @brief Destroy the object data.
             * 
             * @note Frees the object data using the active renderer backend.
             */
            void destroy() {
                if (this->data) {
                    FreeWithActiveBackend(this->data);
                    this->data = nullptr;
                }
            }
            
            /**
             * @brief Initialize the object data.
             * 
             * @note Initializes the object data if the renderer backend is initialized and the data is not already set.
             */
            void initialize() {
                static_assert(std::is_base_of<RaeptorCogs::GAPI::Common::ObjectData, DataType>::value, "DataType must derive from ObjectData");
                if (!this->data) {
                    if (IsRendererBackendInitialized()) {
                        this->data = static_cast<DataType*>(AllocateWithActiveBackend(typeid(DataType)));
                    } else {
                        std::cerr << "Error: Attempted to initialize ObjectHandler before RendererBackend was initialized." << std::endl;
                    }
                }
            }

            /**
             * @brief Get the object data.
             * 
             * @return Pointer to the object data.
             */
            DataType* get() { 
                if (!this->data) {
                    this->initialize();
                }
                return this->data;
            }

            /**
             * @brief Overloaded arrow operator to access object data members.
             * 
             * @return Pointer to the object data.
             */
            DataType* operator->() { return this->get(); }

            /**
             * @brief Get the object data (const version).
             * 
             * @return Pointer to the object data.
             */
            DataType* get() const { 
                return this->data;
            }

            /**
             * @brief Overloaded arrow operator to access object data members (const version).
             * 
             * @return Pointer to the object data.
             */
            const DataType* operator->() const { return this->get(); }

            /**
             * @brief Set the object data.
             * 
             * @param newData Pointer to the new object data.
             * 
             * @note Used internally to set the object data instance.
             */
            void setData(DataType* newData) { this->data = newData; }
    };
};

/**
 * @brief Factory registry declaration macro.
 * 
 * Defines the FactoryRegistry structure and Register template for registering object types.
 * 
 * @note Used internally for type registration and object creation.
 */
#define FACTORY_REGISTRY_DECLARATION() \
    using CreatorFn = Common::ObjectData*(*)();\
    struct FactoryRegistry {\
        static std::unordered_map<std::type_index, CreatorFn>& get() {\
            static std::unordered_map<std::type_index, CreatorFn> map;\
            return map;\
        }\
        template<typename CommonType, typename BackendType>\
        struct _Register {\
            _Register() {\
                FactoryRegistry::get()[typeid(CommonType)] = []() -> Common::ObjectData* {\
                    return new BackendType();\
                };\
            }\
        };\
    };\
    \
    template<typename CommonT, typename ImplT>\
    struct Register {\
        static FactoryRegistry::_Register<CommonT, ImplT> instance;\
    };\
    \
    template<typename CommonT, typename ImplT>\
    FactoryRegistry::_Register<CommonT, ImplT> Register<CommonT, ImplT>::instance;

/**
 * @brief Factory registration macro.
 * 
 * @tparam CommonType Type of the common object.
 * @tparam BackendType Type of the backend object.
 * 
 * @note Used internally to register object types with the factory registry.
 */
#define REGISTER(CommonType, BackendType) template struct Register<CommonType, BackendType>;