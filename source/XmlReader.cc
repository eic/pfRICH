
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#include <string>
#include <vector>

using namespace xercesc;

class XmlTree {
public:
  XmlTree(DOMElement *element): m_Element(element) {
    m_TagName = element->getTagName();

    DOMNodeList *children = element->getChildNodes();
    const XMLSize_t nodeCount = children->getLength();

    for( XMLSize_t xx = 0; xx < nodeCount; ++xx ) {
      DOMNode* currentNode = children->item(xx);
      auto type = currentNode->getNodeType();

      // Yes, for now only interested in these two types;
      if (type == DOMNode::ELEMENT_NODE) {
	auto branch = new XmlTree(dynamic_cast< xercesc::DOMElement* >( currentNode ));
	m_Branches.push_back(branch);
      } 
      else if (type == DOMNode::TEXT_NODE) 
	m_Strings.push_back(dynamic_cast< xercesc::DOMText* >( currentNode ));
    } //for xx
  };
  ~XmlTree() {};

  void Print(unsigned indent = 0) const {
    for(unsigned iq=0; iq<indent; iq++)
      printf(" ");
    printf("%s (%2d)\n", XMLString::transcode(m_TagName), (int)m_Strings.size());
    for(unsigned iq=0; iq<indent; iq++)
      printf(" ");
    for(auto str: m_Strings)
      printf("   %s\n", XMLString::transcode(str->getWholeText()));
      
    for(auto branch: m_Branches)
      branch->Print(indent + 6);
  };

  DOMElement *GetElement( void ) const { return m_Element; };
  const char *GetName( void ) const { return XMLString::transcode(m_TagName); };
  const XMLCh *GetAttribute(const char *attr) {
    return m_Element->getAttribute(XMLString::transcode(attr));
  };

  std::vector<XmlTree*> FindTags(const char *tag) const {
    std::vector<XmlTree*> ret;

    for(auto branch: m_Branches)
      if (XMLString::equals(tag, branch->GetName()))
	ret.push_back(branch);

    return ret;
  };
  XmlTree *FindFirstTag(const char *tag) const {
    for(auto branch: m_Branches)
      if (XMLString::equals(tag, branch->GetName()))
	return branch;

    return 0;
  };
  const char *GetFirstString( void ) const {
    return (m_Strings.size() ? XMLString::transcode(m_Strings[0]->getWholeText()) : 0);
  };

private:
  DOMElement *m_Element;
  const XMLCh *m_TagName;
  std::vector<DOMText*> m_Strings;
  std::vector<XmlTree*> m_Branches;
};

int main()
{
  std::string configFile="sample.xml"; 

  // Initialize Xerces infrastructure
  XMLPlatformUtils::Initialize();  

  auto m_ConfigFileParser = new XercesDOMParser;

  m_ConfigFileParser->parse( configFile.c_str() );
  auto xmlDoc = m_ConfigFileParser->getDocument();

  // Get the top-level element: Name is "Materials";
  DOMElement* elementRoot = xmlDoc->getDocumentElement();

  auto tree = new XmlTree(elementRoot);
  //tree->Print();
  auto mats = tree->FindTags("Material");
  //printf("%d\n", (int)mats.size());
  for(auto mat: mats) {
    {
      const XMLCh* xmlch = mat->GetAttribute("name");
      auto name = XMLString::transcode(xmlch);
      
      auto dens = mat->FindFirstTag("density");
      printf("%s -> %7.4f [g/cm^3]\n", name, atof(dens->GetFirstString()));
    }
    auto comp = mat->FindFirstTag("Components");
    auto elems = comp->FindTags("Element");
    for(auto elem: elems) {
      const XMLCh* xmlch = elem->GetAttribute("fraction");
      auto fraction = XMLString::transcode(xmlch);
      printf("%7.4f: %s\n", atof(fraction), elem->GetFirstString());
    } //for elem

    auto properties = mat->FindTags("Property");
    for(auto prop: properties) {
      const XMLCh* xmlch = prop->GetAttribute("name");
      auto name = XMLString::transcode(xmlch);
      printf("%s\n", name);

      auto values = prop->FindTags("value");
      for(auto value: values) {
	const XMLCh* xmlch = value->GetAttribute("energy");
	auto energy = XMLString::transcode(xmlch);
	printf("%7.2f [eV] -> %8.3f\n", atof(energy), atof(value->GetFirstString()));
      } //for value
    } //for prop
  } //for mat
  //printf("%s\n", mat->GetName());

  return 0;
} //main()

