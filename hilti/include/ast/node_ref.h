// Copyright (c) 2020 by the Zeek Project. See LICENSE for details.

#pragma once

#include <memory>
#include <string>

#include <hilti/base/util.h>

namespace hilti {

class Node;

namespace node_ref {
namespace detail {

// Control block for refering to nodes.
class Control {
public:
    Control(Node* n) : _node(n), _rid(++_rid_counter) {}
    Node* _node;
    uint64_t _rid;

    static uint64_t _rid_counter;
};

} // namespace detail

/** Exceptions thrown when a dangling `NodeRef` is deferenced. */
struct Invalid : std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace node_ref

/**
 * A reference to an AST `Node`. A reference acts similarly to a pointer to a
 * node, except that it tracks its life-time and catches (through exceptions)
 * if the reference becomes invalid. Assigning to a node doesn't affect any
 * references to it; they will now point to the new value.
 */
class NodeRef {
public:
    explicit NodeRef(Node& n); // NOLINT
    explicit NodeRef(const NodeRef& other) = default;
    NodeRef(NodeRef&& other) = default;
    NodeRef& operator=(const NodeRef& other) = default;
    NodeRef& operator=(NodeRef&& other) = default;
    NodeRef() = default;
    ~NodeRef() = default;

    /**
     * Returns the referenced node's unique control ID, or 0 if the instance
     * isn't referencing anything.
     *
     * @note This is primarily for internal usage.
     */
    uint64_t rid() const { return _control ? _control->_rid : 0; }

    /**
     * Returns a string version of the referenced node's unique control ID,
     * or 0 if the instance isn't referencing anything.
     *
     * @note This is primarily for internal usage.
     */
    std::string renderedRid() const { return rid() ? util::fmt("%%%" PRIu64, rid()) : "%???"; };

    /**
     * Returns a pointer to the the referenced node.
     *
     * @exception Invalid if the node does not exist anymore
     */
    const Node* operator->() const { return _node(); }

    /**
     * Returns a direct C++ reference to the the referenced node. The node
     * may be modified, and assigned to, through this reference.
     *
     * @exception Invalid if the node does not exist anymore
     */
    Node& operator*() const { return *_node(); }

    operator const Node&() const { return *_node(); }

    /** Returns true if the instance references a valid node. */
    explicit operator bool() const { return _control && _control->_node; }

private:
    Node* _node() const;
    std::shared_ptr<node_ref::detail::Control> _control;
};

} // namespace hilti
