// $Id:$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DETDESC_XML_DOCUMENTHANDLER_H
#define DETDESC_XML_DOCUMENTHANDLER_H

#include "xercesc/dom/DOM.hpp"
#include "xercesc/sax/ErrorHandler.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "XML/XMLElements.h"
#include <memory>

/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

  /*
  *   XML namespace declaration
  */
  namespace XML  {

    class DocumentHandler {
    public:
      std::auto_ptr<xercesc::ErrorHandler> m_errHdlr;
      DocumentHandler();
      virtual ~DocumentHandler();
      virtual xercesc::XercesDOMParser* makeParser(xercesc::ErrorHandler* err_handler=0) const;
      virtual Document parse(const void* bytes, size_t length) const;
      virtual Document load(const std::string& fname) const;
    };
  }
}         /* End namespace DetDesc   */
#endif    /* DETDESC_XML_DOCUMENTHANDLER_H    */
