// CsvXmlWriter.h
#ifndef have_CsvXmlWriter
#define have_CsvXmlWriter
#include "CsvRewriter.h"

class CsvXmlWriter : public CsvRewriter
{
    void writeXml(FILE *fp, ParsedCsv csv);
    void writeDoctype(FILE *fp, 
                        std::string doctype, 
                        std::vector<std::string> elementNames, 
                        CsvRecord csv);
    void writeXmlRow(FILE *fp, 
                    std::vector<std::string> elementNames, 
                    CsvRecord csv);
    std::vector<std::string> makeXmlElementNames(CsvRecord csv);

public:
    CsvXmlWriter(CsvRewriteParams params);
    virtual ~CsvXmlWriter();
    void write()
    {
        writeXml(m_ofp, m_csv);
    }
};



#endif // have_CsvXmlWriter

