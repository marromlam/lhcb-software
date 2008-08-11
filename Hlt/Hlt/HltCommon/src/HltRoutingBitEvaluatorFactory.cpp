#include "HltRoutingBitEvaluatorFactory.h"
#include "GaudiKernel/ToolFactory.h"
#include "HltBase/HltAlgorithm.h"
#include <boost/spirit.hpp>
#include <boost/spirit/actor.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/ref.hpp>
#include <memory>

using namespace std;
using namespace boost;
using namespace boost::spirit;
using namespace boost::lambda;

namespace {
    // define the elements in the parse tree...

    ostream& operator<<(ostream& o, const HltRoutingBitEvaluatorFactory::evaluator& e) { return e.print(o); }

    class eval_selection: public HltRoutingBitEvaluatorFactory::evaluator {
    public:
        eval_selection(const Hlt::Selection* selection) : m_selection( selection ) {}
        bool evaluate() const  {  return m_selection->decision(); }
        ostream& print(ostream& os) const { return os << m_selection->id().str(); }
    private:
        const Hlt::Selection* m_selection;
    };

    class eval_and : public HltRoutingBitEvaluatorFactory::evaluator {
    public:
        eval_and(HltRoutingBitEvaluatorFactory::evaluator* lhs,
                 HltRoutingBitEvaluatorFactory::evaluator* rhs) : m_lhs(lhs),m_rhs(rhs) {}
        ~eval_and() { }
        bool evaluate()  const  { return m_lhs->evaluate() && m_rhs->evaluate(); }
        ostream& print(ostream& os) const { return os << "and[" << *m_lhs << "," << *m_rhs << "]" ; }
    private:
        auto_ptr<HltRoutingBitEvaluatorFactory::evaluator> m_lhs;
        auto_ptr<HltRoutingBitEvaluatorFactory::evaluator> m_rhs;
    };

    class eval_or : public HltRoutingBitEvaluatorFactory::evaluator {
    public:
        eval_or(HltRoutingBitEvaluatorFactory::evaluator* lhs,
                HltRoutingBitEvaluatorFactory::evaluator* rhs) : m_lhs(lhs),m_rhs(rhs) {}
        ~eval_or() { }
        bool evaluate()  const  { return m_lhs->evaluate() || m_rhs->evaluate(); }
        ostream& print(ostream& os) const { return os << "or[" << *m_lhs << "," << *m_rhs << "]" ; }
    private:
        auto_ptr<HltRoutingBitEvaluatorFactory::evaluator> m_lhs;
        auto_ptr<HltRoutingBitEvaluatorFactory::evaluator> m_rhs;
    };

    class eval_not : public HltRoutingBitEvaluatorFactory::evaluator {
    public:
        eval_not(HltRoutingBitEvaluatorFactory::evaluator* rhs): m_rhs(rhs) {}
        ~eval_not() { }
        bool evaluate()  const  { return !m_rhs->evaluate(); }
        ostream& print(ostream& os) const { return os << "not[" << *m_rhs << "]" ; }
    private:
        auto_ptr<HltRoutingBitEvaluatorFactory::evaluator> m_rhs;
    };

    class evaluator_factory {
    public:
        static HltRoutingBitEvaluatorFactory::evaluator* create(const std::string& input, HltAlgorithm& parent) {
            evaluator_factory p(input,parent); return p.valid() ? p.pop() : (eval*)0 ;
        }
        ~evaluator_factory() { assert(m_stack.empty()); }
    private:
        typedef HltRoutingBitEvaluatorFactory::evaluator eval;
        evaluator_factory(const std::string& input,HltAlgorithm& parent) {
            parent.debug() << "parsing '" << input <<"'" << endreq;

            // TODO simplify using closures/phoenix
            // TODO see http://www.boost.org/doc/libs/1_34_1/libs/spirit/doc/closures.html

            // define our grammar, 
            // and associate parser actions
            rule<phrase_scanner_t> decision, term, unary, expression;
            term       = (+alnum_p)                         [ bind(&evaluator_factory::op_push,this,ref(parent),_1,_2)] 
                       | '(' >> expression >> ')';
            unary      = term |  ('!'>>term)                [ bind(&evaluator_factory::op_not,this,_1,_2) ];
            expression = unary >> *( ( '|' >> unary )       [ bind(&evaluator_factory::op_or ,this,_1,_2) ]
                                   | ( '&' >> unary )       [ bind(&evaluator_factory::op_and,this,_1,_2) ]
                                   );
            parse_info<> info = parse( input.c_str(), expression, space_p  );

            m_valid = info.full;
            if (!m_valid) {
                parent.warning() << "Parsing failed at:" << info.stop << endreq;
            }
        }

        bool valid() const { return m_valid;}

        evaluator_factory& op_push(HltAlgorithm& parent,const char* begin, const char *end) {
           return push(new eval_selection(&parent.retrieveSelection(string(begin,end))));
        }
        evaluator_factory& op_or(const char*,const char*) {
           eval* lhs = pop(); eval* rhs = pop(); 
           return push( new eval_or(lhs,rhs) );
        }
        evaluator_factory& op_and(const char*,const char*) {
           eval* lhs = pop(); eval* rhs = pop(); 
           return push( new eval_and(lhs,rhs) );
        }
        evaluator_factory& op_not(const char*,const char*) {
           return push( new eval_not( pop() ) );
        }

        eval* pop() { assert(!m_stack.empty()); eval* t = m_stack.back(); m_stack.pop_back(); return t; }
        evaluator_factory& push(eval* t) { m_stack.push_back(t); return *this;}

        std::vector<eval*> m_stack;
        bool m_valid;
    };

}

HltRoutingBitEvaluatorFactory::evaluator::~evaluator() 
{}


DECLARE_TOOL_FACTORY( HltRoutingBitEvaluatorFactory );

HltRoutingBitEvaluatorFactory::HltRoutingBitEvaluatorFactory( const std::string& type,
                                                              const std::string& name,
                                                              const IInterface* parent)
    : GaudiTool(type,name,parent)                                                                
{ 
    declareInterface<HltRoutingBitEvaluatorFactory>(this);
}

HltRoutingBitEvaluatorFactory::~HltRoutingBitEvaluatorFactory()
{ }

HltRoutingBitEvaluatorFactory::evaluator* 
HltRoutingBitEvaluatorFactory::create(const std::string& specification, HltAlgorithm& parent) const {
   return evaluator_factory::create(specification,parent); 
}
