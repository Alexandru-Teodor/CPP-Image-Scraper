#include <iostream>
#include "cpr/cpr.h"
#include "libxml/HTMLparser.h"
#include "libxml/xpath.h"
#include <vector>

// define a struct where to store the scraped data
struct IndustryCard
{
    std::string image;
    std::string url;
    std::string name;
};

int main()
{
    // define the user agent for the GET request
    cpr::Header headers = {{"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/113.0.0.0 Safari/537.36"}};
    // make an HTTP GET request to retrieve the target page
    cpr::Response response = cpr::Get(cpr::Url{"https://scrapeme.live/shop/"}, headers);
    
    // parse the HTML document returned by the server
    htmlDocPtr doc = htmlReadMemory(response.text.c_str(), response.text.length(), nullptr, nullptr, HTML_PARSE_NOWARNING | HTML_PARSE_NOERROR);
    
    // define an array to store all retrieved data
    std::vector<IndustryCard> industry_cards;
    // set the libxml2 context to the current document
    xmlXPathContextPtr context = xmlXPathNewContext(doc);
    // select all industry card HTML elements
    // with an XPath selector
    xmlXPathObjectPtr industry_card_html_elements = xmlXPathEvalExpression((xmlChar *)"//div[contains(@class, 'section_cases_row_col_item')]", context);

    // iterate over the list of industry card elements
    for (int i = 0; i < industry_card_html_elements->nodesetval->nodeNr; ++i)
    {
        // get the current element of the loop
        xmlNodePtr industry_card_html_element = industry_card_html_elements->nodesetval->nodeTab[i];
        // set the libxml2 context to the current element
        // to limit the XPath selectors to its children
        xmlXPathSetContextNode(industry_card_html_element, context);

        xmlNodePtr image_html_element = xmlXPathEvalExpression((xmlChar *)".//figure/a/img", context)->nodesetval->nodeTab[0];
        std::string image = std::string(reinterpret_cast<char *>(xmlGetProp(image_html_element, (xmlChar *)"data-lazy-src")));

        xmlNodePtr url_html_element = xmlXPathEvalExpression((xmlChar *)".//figure/a", context)->nodesetval->nodeTab[0];
        std::string url = std::string(reinterpret_cast<char *>(xmlGetProp(url_html_element, (xmlChar *)"href")));

        xmlNodePtr name_html_element = xmlXPathEvalExpression((xmlChar *)".//div[contains(@class, 'elementor-image-box-title')]/a", context)->nodesetval->nodeTab[0];
        std::string name = std::string(reinterpret_cast<char *>(xmlNodeGetContent(name_html_element)));

        // instantiate an IndustryCard struct with the collected data
        IndustryCard industry_card = {image, url, name};
        // add the object with the scraped data to the vector
        industry_cards.push_back(industry_card);
    }

    // free up the resource allocated by libxml2
    xmlXPathFreeObject(industry_card_html_elements);
    xmlXPathFreeContext(context);
    xmlFreeDoc(doc);
    for (IndustryCard industry_card : industry_cards)
    {
        std::cout << industry_card.image << '\n';
    }
    std::cout << "--debug message--\n";
    return 0;
}