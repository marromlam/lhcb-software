#ifndef CONFIGTREE_H
#define CONFIGTREE_H 1

#include "LHCbMath/MD5.h"
#include <iostream>
#include <vector>
#include "boost/optional.hpp"

class ConfigTreeNode {
public:
    // this class represent an adjacency view layout of the configuration tree...
    // There is one 'leaf' per node: basically, the 'data' at this node,
    // (typically a PropertyConfig instance)
    // and zero or several 'edges' which point to nodes on which this node
    // depends (i.e. edges are ConfigTreeNode instances).
    //
    // 'references' to both vertices and edges are represented by the
    // MD5 hash of their canonical string representation, i.e. given
    // an MD5 hash of a leaf, one is assumed to be able to fetch the 
    // corresponding data, in this case a 'PropertyConfig' instance;
    // given an MD5 hash of an edge, one is assumed to be able to fetch the 
    // corresponding 'ConfigTreeNode' instance.
    //
    // Fetching the corresponding data is factored out of this class --
    // use an instance of the IConfigAccessSvc to do that.
    //
    typedef Gaudi::Math::MD5::Digest digest_type;
    typedef digest_type              LeafRef;
    typedef digest_type              NodeRef;
    typedef std::vector<NodeRef>     NodeRefs;

    ConfigTreeNode() : m_leaf( Gaudi::Math::MD5::createInvalidDigest() ) 
    { }

    ConfigTreeNode(const LeafRef& leaf)
      : m_leaf(leaf)
    { }

    ConfigTreeNode(const LeafRef& leaf, const NodeRefs& nodes)
      : m_nodes(nodes)
      , m_leaf(leaf)
    { }

    ConfigTreeNode(const LeafRef& leaf, const NodeRefs& nodes, const std::string& label)
      : m_nodes(nodes)
      , m_leaf(leaf)
      , m_label(label)
    { }

    ConfigTreeNode *clone(boost::optional<const LeafRef&> newLeaf = boost::optional<const LeafRef&>(), 
                          boost::optional<const NodeRefs&> newNodes = boost::optional<const NodeRefs&>(), 
                          boost::optional<const std::string&> newLabel = boost::optional<const std::string&>()) const {
        return new ConfigTreeNode( newLeaf  ? newLeaf.get()  : m_leaf,
                                   newNodes ? newNodes.get() : m_nodes,
                                   newLabel ? newLabel.get() : m_label );
    }

    bool operator==(const ConfigTreeNode& rhs) const { 
        return m_leaf == rhs.m_leaf 
            && m_label == rhs.m_label // should we require the labels to match as well???
                                      // They are part of the MD5, so if we don't, we 
                                      // might get the situation that == says yes, but
                                      // the MD5s are different.... Probably don't want
                                      // that to happen...
            && m_nodes == rhs.m_nodes; 
    }

    digest_type digest() const;

    std::ostream& print(std::ostream& os) const;
    std::istream& read(std::istream& is);

    const NodeRefs& nodes() const  { return m_nodes; }
    const LeafRef&  leaf() const { return m_leaf; }
    const std::string& label() const { return m_label; }

private:
    NodeRefs    m_nodes;
    LeafRef     m_leaf;
    std::string m_label;
};

std::ostream& operator<<(std::ostream& os, const ConfigTreeNode& x);
std::istream& operator>>(std::istream& is, ConfigTreeNode& x);

#endif
