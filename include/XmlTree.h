
#include <vector>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#ifndef _XML_TREE_
#define _XML_TREE_

class XmlTree {
public:
 XmlTree(xercesc::DOMElement *element): m_Element(element) {
    m_TagName = element->getTagName();

    xercesc::DOMNodeList *children = element->getChildNodes();
    const XMLSize_t nodeCount = children->getLength();

    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
      xercesc::DOMNode* currentNode = children->item(xx);
      auto type = currentNode->getNodeType();

      // Yes, for now only interested in these two types;
      if (type == xercesc::DOMNode::ELEMENT_NODE) {
	auto branch = new XmlTree(dynamic_cast< xercesc::DOMElement* >( currentNode ));
	m_Branches.push_back(branch);
      } 
      else if (type == xercesc::DOMNode::TEXT_NODE) 
	m_Strings.push_back(dynamic_cast< xercesc::DOMText* >( currentNode ));
    } //for xx
  };
  ~XmlTree() {};

  void Print(unsigned indent = 0) const {
    for(unsigned iq=0; iq<indent; iq++)
      printf(" ");
    printf("%s (%2d)\n", xercesc::XMLString::transcode(m_TagName), (int)m_Strings.size());
    for(unsigned iq=0; iq<indent; iq++)
      printf(" ");
    for(auto str: m_Strings)
      printf("   %s\n", xercesc::XMLString::transcode(str->getWholeText()));
      
    for(auto branch: m_Branches)
      branch->Print(indent + 6);
  };

  xercesc::DOMElement *GetElement( void ) const { return m_Element; };
  const char *GetName( void ) const { return xercesc::XMLString::transcode(m_TagName); };
  const XMLCh *GetAttribute(const char *attr) {
    return m_Element->getAttribute(xercesc::XMLString::transcode(attr));
  };

  std::vector<XmlTree*> FindTags(const char *tag) const {
    std::vector<XmlTree*> ret;

    for(auto branch: m_Branches)
      if (xercesc::XMLString::equals(tag, branch->GetName()))
	ret.push_back(branch);

    return ret;
  };
  XmlTree *FindFirstTag(const char *tag) const {
    for(auto branch: m_Branches)
      if (xercesc::XMLString::equals(tag, branch->GetName()))
	return branch;

    return 0;
  };
  const char *GetFirstString( void ) const {
    return (m_Strings.size() ? xercesc::XMLString::transcode(m_Strings[0]->getWholeText()) : 0);
  };

private:
  xercesc::DOMElement *m_Element;
  const XMLCh *m_TagName;
  std::vector<xercesc::DOMText*> m_Strings;
  std::vector<XmlTree*> m_Branches;
};

#endif
