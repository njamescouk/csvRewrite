// CsvJsonWriter.h
#ifndef have_CsvJsonWriter
#define have_CsvJsonWriter
#include "CsvRewriter.h"

class CsvJsonWriter : public CsvRewriter
{
    void writeJson(FILE *fp, ParsedCsv csv);
    void writeJsonRow(FILE *fp, 
                    std::vector<std::string> elementNames, 
                    CsvRecord csv);

public:
    CsvJsonWriter(CsvRewriteParams params);
    virtual ~CsvJsonWriter();
    void write()
    {
        writeJson(m_ofp, m_csv);
    }
};



#endif // have_CsvJsonWriter

