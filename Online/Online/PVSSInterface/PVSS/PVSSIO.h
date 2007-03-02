// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/PVSSIO.h,v 1.1 2007-03-02 19:54:04 frankb Exp $
//  ====================================================================
//  Array.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_ARRAYIO_H
#define ONLINE_PVSS_ARRAYIO_H

// Framework include files
#include "PVSS/Array.h"
class Variable;
/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  // Forward declarations
  class Value;

  void setGenWriteIO(void* context,void*& listCtxt,const DpIdentifier& dp,const Value* val);
  template <class T> void setWriteIO(void* context,void*& listCtxt,int typ,const DpIdentifier& dp,const std::vector<T>& v);

  void genReadIO(const Variable* variable,int typ, void* buff);
  template <class T> void addWriteIO(void* context,int typ,const DpIdentifier& dp,const std::vector<T>& v);
}      // End namespace PVSS
#endif // ONLINE_PVSS_BITALARM_H
