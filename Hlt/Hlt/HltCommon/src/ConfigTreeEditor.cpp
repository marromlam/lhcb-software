// Include files
// local
#include "ConfigTreeEditor.h"

// std
#include <memory>

// boost
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/construct.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/ref.hpp"
#include "boost/regex.hpp"



// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from HltBase

using namespace std;
using namespace boost;
using namespace boost::lambda;

namespace {
    static const std::string empty;
};

/////////Utility class for easy manipulation of configurations/////////////////////////////
class ConfigTree {
public:

   class Visitor {
   public: 
      virtual ~Visitor() {};
      virtual void pre(const ConfigTree&) { ; }
      virtual void post(const ConfigTree&) { ; }
      virtual bool descend(const ConfigTree&) { return true; }
   };

   class Lookup { // contains all object for this tree...
   public:
        Lookup(IPropertyConfigSvc& r) : m_r(r) {};
        const ConfigTreeNode *node(const ConfigTreeNode::digest_type& id) {
            NodeMap_t::iterator i = m_nodes.find(id);
            if (i==m_nodes.end()) {
                     const ConfigTreeNode *cn = m_r.resolveConfigTreeNode(id);
                     pair<NodeMap_t::iterator,bool> r = m_nodes.insert(make_pair( id, cn));
                     assert(r.second);
                     i = r.first;
            }
            return i->second;
        }

        const PropertyConfig *leaf(const PropertyConfig::digest_type& id) {
            ConfigMap_t::iterator i = m_config.find(id);
            if (i==m_config.end()) {
                     const PropertyConfig *pc = m_r.resolvePropertyConfig(id);
                     pair<ConfigMap_t::iterator,bool> r = m_config.insert(make_pair( id, pc));
                     assert(r.second);
                     i = r.first;
            }
            return i->second;
        }
        ConfigTree     *tree(const ConfigTreeNode::digest_type& id, ConfigTree* parent) {
            TreeMap_t::iterator i = m_trees.find(id);
            if (i==m_trees.end()) {
                 pair<TreeMap_t::iterator,bool> r = m_trees.insert( make_pair( id, new ConfigTree( id, parent) ) );
                 assert(r.second);
                 i = r.first;
            } else {
                 i->second->addParent(parent);
            }
            return i->second;
        }
   private:
        typedef map<ConfigTreeNode::digest_type,ConfigTree*>  TreeMap_t;
        typedef map<PropertyConfig::digest_type,const PropertyConfig*> ConfigMap_t;
        typedef map<ConfigTreeNode::digest_type,const ConfigTreeNode*>  NodeMap_t;
        TreeMap_t                      m_trees;
        ConfigMap_t                    m_config;
        NodeMap_t                      m_nodes;
        IPropertyConfigSvc&            m_r;
   };

   // constructor for the nodes inside the tree
   ConfigTree(const ConfigTreeNode::digest_type& in, ConfigTree* parent  ) 
    : m_lookup(0)
    , m_ownedLeaf(0)
    , m_root(0)
    , m_leaf(0)
    , m_origDigest(in)
   {
        assert(parent!=0);
        addParent(parent);
        addLeaf();
        addDeps();
   }


   // constructor for the top of the tree
         // root node is special: 
         // given that the graph might contain cycles, we need one
         // lookup table which is 'global' to the tree -- so we put
         // it into the 'root' node.
   ConfigTree(const ConfigTreeNode::digest_type& in, IPropertyConfigSvc& r, const std::string& label = empty )
    : m_lookup( new ConfigTree::Lookup(r) )
    , m_ownedLeaf(0)
    , m_root(this)
    , m_leaf(0)
    , m_origDigest(in)
   {
        addParent((ConfigTree*)0);
        addLeaf();
        if (!label.empty()) m_label = label; // update label!
        else { 
            m_label += " ( mutation of " + in.str() + " )";
        }
        addDeps();
   }

   void addParent(ConfigTree *parent) {
        if (parent!=0) { 
            assert( root()==0 ||  root() == parent->root() ); // must be same tree...
            assert(find(m_parents.begin(),m_parents.end(),parent)==m_parents.end());
            m_parents.push_back(parent);
            assert(m_lookup.get()==0);
            m_root = parent->root();
        } else {
            assert(m_parents.empty());
        }
   }

   void addLeaf() {
        const ConfigTreeNode *c = lookupConfigTreeNode( m_origDigest );
        assert(c!=0);
        m_label = c->label();
        if ( !c->leaf().invalid() ) {
            m_leaf = lookupPropertyConfig( c->leaf() );
            assert(m_leaf!=0);
        }
   }

   void addDeps() {
        const ConfigTreeNode *c = lookupConfigTreeNode( m_origDigest );
        for (ConfigTreeNode::NodeRefs::const_iterator i = c->nodes().begin(); i != c->nodes().end(); ++i ) {
            m_deps.push_back( lookupConfigTree( *i, this ) );  // lookup returns a pointer to either a 'new' ConfigTree instance,
                                                             // or to an existing one, adding 'this' to the list of parents of it.
                                                             // It is done this way because the graph may contain cycles...
        }

   }

   ~ConfigTree() { }

   void visit( ConfigTree::Visitor& visitor ) const {
        visitor.pre(*this);
        if (visitor.descend(*this)) {
              for_each(m_deps.begin(),
                       m_deps.end(),
                       bind(&ConfigTree::visit,_1,ref(visitor)));
        }
        visitor.post(*this);
   }

   ConfigTreeNode::digest_type write(IConfigAccessSvc& w) {
       ConfigTreeNode::LeafRef  lr = ( m_leaf == 0 ? ConfigTreeNode::digest_type::createInvalid()
                                                   : w.writePropertyConfig(*m_leaf) );
       assert(m_leaf==0||!lr.invalid());
       ConfigTreeNode::NodeRefs nr;
       for (vector<ConfigTree*>::const_iterator i = m_deps.begin(); i!= m_deps.end(); ++i ) {
            ConfigTreeNode::digest_type d = (*i)->write(w);
            assert(!d.invalid());
            nr.push_back(d);
       }
       const ConfigTreeNode *c = lookupConfigTreeNode( m_origDigest );
       assert(c!=0);
       auto_ptr<ConfigTreeNode> nc( c->clone( lr,nr, m_label ) );
       return w.writeConfigTreeNode( *nc );
   }

   const ConfigTreeNode* lookupConfigTreeNode(const ConfigTreeNode::digest_type& id ) {
        return lookup().node(id);
   }
   const PropertyConfig* lookupPropertyConfig(const PropertyConfig::digest_type& id ) {
        return lookup().leaf(id);
   }
   ConfigTree* lookupConfigTree(const ConfigTreeNode::digest_type& id, ConfigTree* parent ) {
        return lookup().tree(id,parent);
   }
   
   ConfigTree* findNodeWithLeaf(const string& name) {
       if ( m_leaf && m_leaf->name() == name )  return this;
       for ( vector<ConfigTree*>::const_iterator i  = m_deps.begin(); i!=m_deps.end();++i) {  
            ConfigTree *f = (*i)->findNodeWithLeaf(name);
            if (f!=0) return f;
       }
       return 0;
   }

   const PropertyConfig* leaf() const { return m_leaf; }

   template <typename T>
   bool updateLeaf(const T& key2value) { // T::value_type must be pair<string,string>, representing (key,value)
           PropertyConfig::Properties props = m_leaf->properties();
           for (typename T::const_iterator j=key2value.begin();j!=key2value.end();++j) {
               PropertyConfig::Properties::iterator i = find_if( props.begin(),  
                                                                 props.end(),
                                                                 bind(&PropertyConfig::Prop::first,_1) == j->first );
               if (i==props.end()) {
                    cerr << "trying to update a non-existing property: " << j->first << endl;
                    return false;
               }
               *i = *j;
           }
           m_ownedLeaf.reset( m_leaf->clone(props) );
           m_leaf = m_ownedLeaf.get();
           return true;
   }

private:
   ConfigTree* root() { return m_root; }

   Lookup& lookup() {
        assert( (m_root==this && m_lookup.get()!=0 ) || (m_root!=this && m_lookup.get()==0 ));
        return m_lookup.get()!=0 ? *m_lookup : root()->lookup();
   }

   auto_ptr<Lookup>               m_lookup;
   auto_ptr<PropertyConfig>       m_ownedLeaf;
   vector<ConfigTree*>            m_parents;
   vector<ConfigTree*>            m_deps;
   vector<ConfigTree*>            m_ownedDeps;
   ConfigTree                    *m_root;
   const PropertyConfig          *m_leaf;
   ConfigTreeNode::digest_type    m_origDigest;
   string                         m_label;
};
///////////////////////////////////////////////////////////////////////////////////////////

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ConfigTreeEditor );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ConfigTreeEditor::ConfigTreeEditor( const string& type,
                                    const string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_propertyConfigSvc(0)
  , m_configAccessSvc(0)
{
  declareInterface<IConfigTreeEditor>(this);
  declareProperty( "ConfigAccessSvc",  s_configAccessSvc = "ConfigFileAccessSvc"  );
  declareProperty( "PropertyConfigSvc",  s_propertyConfigSvc = "PropertyConfigSvc"  );
}
//=============================================================================
// Destructor
//=============================================================================
ConfigTreeEditor::~ConfigTreeEditor() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ConfigTreeEditor::initialize() {
  info() << "Initialize" << endmsg;
  StatusCode status = GaudiTool::initialize();
  if (!status.isSuccess()) return status;
  m_configAccessSvc = svc<IConfigAccessSvc>(s_configAccessSvc);
  m_propertyConfigSvc = svc<IPropertyConfigSvc>(s_propertyConfigSvc);
  return StatusCode::SUCCESS;
}
//=============================================================================
// Finalization
//=============================================================================
StatusCode ConfigTreeEditor::finalize() {
  info() << "Finalize" << endmsg;
  return GaudiTool::finalize();
}


ConfigTreeNode::digest_type
ConfigTreeEditor::updateAndWrite(const ConfigTreeNode::digest_type& in,
                                 const multimap<string,pair<string,string> >& updates,
                                 const string& label) const
{
   ConfigTree tree( in, *m_propertyConfigSvc, label );
   typedef multimap<string,pair<string,string> > map_t;
   map_t::const_iterator i = updates.begin();
   while ( i!=updates.end() ) {
         ConfigTree* node = tree.findNodeWithLeaf(i->first);
         // grab entire range matching this one
         map_t::const_iterator j = updates.upper_bound(i->first);
         vector<pair<string,string> > mods;
         for (;i!=j;++i) mods.push_back(i->second);
         node->updateLeaf(mods);
   }
   return tree.write( *m_configAccessSvc );
}

ConfigTreeNode::digest_type
ConfigTreeEditor::updateAndWrite(const ConfigTreeNode::digest_type& in,
                                 const vector<string>& updates,
                                 const string& label) const {

   multimap<string,pair<string,string> >  mm;
   for (vector<string>::const_iterator i = updates.begin(); i!=updates.end(); ++i) {
       string::size_type c = i->find(':');
       string lhs = i->substr(0,c);
       string rhs = i->substr(c+1,string::npos);
       string::size_type d = lhs.rfind('.');
       string comp = lhs.substr(0,d);
       string key  = lhs.substr(d+1,string::npos);
       mm.insert( make_pair( comp, make_pair(key, rhs ) ) );
   }
   return updateAndWrite( in, mm, label );
}






class TreePrinter : public ConfigTree::Visitor {
public:
   TreePrinter(std::ostream& os,bool algs, bool svcs, bool tools) 
     : m_os(os)
     , m_doAlgs(algs)
     , m_doSvcs(svcs)
     , m_doTools(tools) 
     , m_depth(0)
   {}

   void pre(const ConfigTree& node) { 
      if (match(node)) m_os << std::string(1+3*m_depth,' ') 
                            << node.leaf()->name() << endl;
   }

   bool descend(const ConfigTree&) { ++m_depth; return true; }

   void post(const ConfigTree&) { assert(m_depth>0); --m_depth; }

private:
   bool match(const ConfigTree& node) const {
       const PropertyConfig* leaf = node.leaf();
       return leaf != 0 && (
                (leaf->kind() == "IAlgorithm" && m_doAlgs) 
              ||(leaf->kind() == "IService" && m_doSvcs)
              ||(leaf->kind() == "IAlgTool" && m_doTools) ) ;
   }
   std::ostream&  m_os;
   bool           m_doAlgs;
   bool           m_doSvcs;
   bool           m_doTools;
   int            m_depth;
};


void
ConfigTreeEditor::printAlgorithms(const ConfigTreeNode::digest_type& in) const
{
   TreePrinter x(cout, true,false,false);
   ConfigTree( in, *m_propertyConfigSvc ).visit( x );
}

void
ConfigTreeEditor::printServices(const ConfigTreeNode::digest_type& in) const
{
   TreePrinter x(cout, false,true,false);
   ConfigTree( in, *m_propertyConfigSvc ).visit( x );
}

void
ConfigTreeEditor::printTools(const ConfigTreeNode::digest_type& in) const
{
   TreePrinter x(cout, false,false,true);
   ConfigTree( in, *m_propertyConfigSvc ).visit( x );
}


class PropPrinter : public ConfigTree::Visitor {
public:
   PropPrinter(std::ostream& os,const std::string& comp, const std::string& prop) 
     : m_os(os)
     , m_comp(comp.empty()? ".*" : comp)
     , m_prop(prop.empty()? ".*" : prop)
   {}

   void pre(const ConfigTree& node) { 
      if (!match(node)) return;
      bool first=true;
      const PropertyConfig::Properties& props = node.leaf()->properties();
      for (PropertyConfig::Properties::const_iterator i = props.begin();
           i!=props.end();++i) {
             if (!match(i->first)) continue;
             if (first) {
                    m_os << "\n    Requested Properties for " 
                         << node.leaf()->fullyQualifiedName() << "\n";
                    first = false;
             }
             m_os << "     \'" << i->first << "\': " << i->second << endl;
      }
   }

   bool descend(const ConfigTree&) { return true; }

   void post(const ConfigTree&) { }

private:
   bool match(const ConfigTree& node) const {
       const PropertyConfig* leaf = node.leaf();
       boost::smatch what;
       return leaf != 0 && ( boost::regex_match(leaf->name(),what,m_comp) );
   }
   bool match(const string& key) const {
       boost::smatch what;
       return boost::regex_match(key,what,m_prop);
   }

   std::ostream&  m_os;
   boost::regex   m_comp,m_prop;
};

void
ConfigTreeEditor::printProperties(const ConfigTreeNode::digest_type& in, const std::string& comp,
                                                                         const std::string& prop) const
{
   PropPrinter x(cout, comp, prop );
   ConfigTree( in, *m_propertyConfigSvc ).visit( x );
}




class DiffFinder : public ConfigTree::Visitor {
public:
   DiffFinder(ConfigTree* referenceTree); 

   void pre(const ConfigTree& ) { }

   bool descend(const ConfigTree&) { return true;}

   void post(const ConfigTree&) { }

private:
};
