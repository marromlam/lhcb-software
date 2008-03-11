// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/OMAlib/OMAalg.h,v 1.3 2008-03-11 18:23:26 ggiacomo Exp $
#ifndef OMALIB_OMAALG_H
#define OMALIB_OMAALG_H 1
/** @class  OMAalg OMAalg.h OMAlib/OMAalg.h
 * 
 *  generic Online Monitoring Analysis algorithm
 *  @author Giacomo Graziani (INFN Firenze)
 *  @date 3/10/2007
 */
#include "OnlineHistDB/OnlineHistDB.h"
#include "OMAlib/OMAMsgInterface.h"
#include <TH1.h>
using namespace std;

class OMAalg : public OMAMsgInterface
{
 public:
  OMAalg(std::string Name);
  virtual ~OMAalg();
  inline std::string name() const { return m_name;}
  std::string type();
  inline int npars() const {return m_npars;}
  inline int ninput() const {return m_ninput;}
  inline std::string doc() const {return m_doc;}
  inline std::string parName(int i) const { return m_parnames[i];}

 protected:
  void setType(std::string type);
  inline void setNpars(int N) {m_npars = N;}
  void setNinput(int N) {m_ninput = N;}
  inline void setDoc(std::string Doc) {m_doc = Doc;}
  void setParNames(std::vector<std::string> &ParNames);
  virtual void raiseMessage(OMAMsgLevel level,
                            std::string message,
                            std::string histogramName );

  std::string m_name;
  typedef enum { HCREATOR, CHECK} AlgType;
  AlgType m_type;
  unsigned int m_npars;
  unsigned int m_ninput;
  std::vector<std::string> m_parnames;
  std::string m_doc;

 private:
   // private dummy copy constructor and assignment operator 
  OMAalg(const OMAalg&): OMAMsgInterface() {}
  OMAalg & operator= (const OMAalg&) {return *this;}


};

class OMACheckAlg : public OMAalg
{
 public:
  OMACheckAlg(std::string Name) : OMAalg(Name) {setType("CHECK");}
  virtual ~OMACheckAlg() {}
  inline std::string inputName(int i) const { return m_inputNames[i];}

  virtual void exec(TH1 &Histo,
                    std::vector<float> & warn_thresholds,
                    std::vector<float> & alarm_thresholds,
                    std::vector<float> & input_pars) =0;

 protected:
  std::vector<std::string> m_inputNames;
 private:
   // private dummy copy constructor and assignment operator 
  OMACheckAlg(const OMACheckAlg&) : OMAalg(m_name) {}
  OMACheckAlg & operator= (const OMACheckAlg&) {return *this;}
};


class OMAHcreatorAlg : public OMAalg
{
 public:
  OMAHcreatorAlg(std::string Name) : OMAalg(Name) {setType("HCREATOR");}
  virtual ~OMAHcreatorAlg() {}
  
  virtual TH1*  exec( const std::vector<TH1*> *sources,
                      const std::vector<float> *params,
                      std::string outName,
                      std::string outTitle,
                      TH1* existingHisto=0) =0;
  inline OnlineHistDBEnv::HistType outHType() const {return m_outHType;}
  inline bool histSetFlag() { return m_histSetFlag; }
  void setHistType(OnlineHistDBEnv::HistType htype) {m_outHType = htype;}
  void setHistSetFlag(bool Flag=true) { m_histSetFlag= Flag; }

 protected:
  OnlineHistDBEnv::HistType m_outHType;
  bool m_histSetFlag;

 private:
   // private dummy copy constructor and assignment operator 
  OMAHcreatorAlg(const OMAHcreatorAlg&) : OMAalg(m_name) {}
  OMAHcreatorAlg & operator= (const OMAHcreatorAlg&) {return *this;}


};
#endif // OMALIB_OMAALG_H
