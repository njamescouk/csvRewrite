// CsvHtmlWriter.h
#ifndef have_CsvHtmlWriter
#define have_CsvHtmlWriter

#include "CsvRewriter.h"

class CsvHtmlWriter : public CsvRewriter
{
    void writeHtml(FILE *fp, ParsedCsv csv);
    void writeHeader(FILE *fp, CsvRecord csv);
    void writeTable(FILE *fp, ParsedCsv csv);

public:
    CsvHtmlWriter(CsvRewriteParams params);
    virtual ~CsvHtmlWriter();
    void write()
    {
        writeHtml(m_ofp, m_csv);
    }
};



#endif // have_CsvHtmlWriter

