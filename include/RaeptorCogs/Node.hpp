/** ********************************************************************************
 * @section Node_Overview Overview
 * @file Node.hpp
 * @brief High-level node management utilities.
 * @details
 * Typical use cases:
 * - Managing hierarchical node structures.
 * *********************************************************************************
 * @section Node_Header Header
 * <RaeptorCogs/Node.hpp>
 ***********************************************************************************
 * @section Node_Metadata Metadata
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
#include <RaeptorCogs/BitArray.hpp>
#include <vector>

namespace RaeptorCogs {

/**
 * @brief Node class.
 * 
 * Represents a node in a hierarchical structure and provides utilities for managing
 * parent-child relationships and type identification.
 * 
 * @code{.cpp}
 * class MyNode : public RaeptorCogs::RegisterNode<MyNode, RaeptorCogs::Node> {};
 * RaeptorCogs::Node* root = new RaeptorCogs::Node();
 * RaeptorCogs::Node* child = new MyNode();
 * root->addChild(child);
 * bool isMyNode = child->isInstanceOf<MyNode>(); // true
 * @endcode
 */
class Node {
    private:

        // ============================================================================
        //                               PRIVATE ATTRIBUTES
        // ============================================================================

        /**
         * @brief Parent node pointer.
         * 
         * Points to the parent of this node in the hierarchy.
         */
        Node* parent = nullptr;

        /**
         * @brief Child nodes vector.
         * 
         * Stores pointers to the child nodes of this node.
         */
        std::vector<Node*> children;

        /**
         * @brief Get the next unique class ID.
         * 
         * @return The next unique class ID.
         */
        static size_t getNextClassId() {
            static size_t id = 0;
            return id++;
        }

    protected:

        // ============================================================================
        //                               PROTECTED METHODS
        // ============================================================================

        /**
         * @brief Set the parent node.
         * 
         * @param parent Pointer to the parent node.
         * 
         * @note Called internally when adding/removing child nodes.
         */
        virtual void setParent(Node* parent);

    public:

        // ============================================================================
        //                               PUBLIC METHODS
        // ============================================================================

        /**
         * @brief Default constructor for Node.
         */
        Node() = default;

        /**
         * @brief Destructor for Node.
         */
        virtual ~Node();

        /**
         * @brief Get the class IDs BitArray for this node.
         * 
         * @return BitArray representing the class IDs of this node.
         */
        virtual BitArray getClassIds() const {
            BitArray classIds;
            return classIds.set(getClassId<Node>());
        }

        /**
         * @brief Check if this node is an instance of the specified type.
         * 
         * @tparam T The type to check against.
         * @return true if this node is an instance of type T, false otherwise.
         * 
         * @code{.cpp}
         * bool isMyNode = node->isInstanceOf<MyNode>(); // true if node is of type MyNode
         * @endcode
         */
        template<typename T>
        bool isInstanceOf() const {
            return this->getClassIds().test(Node::getClassId<T>());
        }

        /**
         * @brief Add a child node.
         * 
         * @param child Pointer to the child node to add.
         * 
         * @note Sets the parent of the child node to this node.
         */
        void addChild(Node* child);

        /**
         * @brief Remove a child node.
         * 
         * @param child Pointer to the child node to remove.
         * 
         * @note Sets the parent of the child node to nullptr.
         */
        void removeChild(Node* child);

        /**
         * @brief Get the child nodes.
         * 
         * @return Vector of pointers to the child nodes.
         * 
         * @note The returned vector is const to prevent modification of the child nodes.
         */
        const std::vector<Node*>& getChildren() const;

        /**
         * @brief Get the parent node.
         * 
         * @return Pointer to the parent node, or nullptr if this node has no parent.
         */
        Node* getParent() const;

        /**
         * @brief Get the unique class ID for a specific type.
         * 
         * @tparam T The type to get the class ID for.
         * @return The unique class ID for type
         * 
         * @note Each type T will have a unique class ID assigned the first time this method is called.
         */
        template<typename T>
        static size_t getClassId() {
            static size_t classId = getNextClassId();
            return classId;
        }
};

/**
 * @brief RegisterNode struct.
 * 
 * Template struct to register a derived node type and update its class IDs.
 * 
 * @tparam Derived The derived node type.
 * @tparam Base The base node type.
 * 
 * @code{.cpp}
 * class MyNode : public RaeptorCogs::RegisterNode<MyNode, RaeptorCogs::Node> {};
 * @endcode
 */
template<typename Derived, typename Base>
struct RegisterNode : Base {

    /**
     * @brief Get the class IDs BitArray for this node.
     * 
     * @return BitArray representing the class IDs of this node.
     */
    BitArray getClassIds() const override {
        BitArray bits = Base::getClassIds();
        bits.set(Node::getClassId<Derived>());
        return bits;
    }
};

}