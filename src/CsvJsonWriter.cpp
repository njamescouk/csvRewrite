#include "CsvJsonWriter.h" 

CsvJsonWriter::CsvJsonWriter(CsvRewriteParams params)
    : CsvRewriter(params) 
{
}

CsvJsonWriter::~CsvJsonWriter() 
{
}


void CsvJsonWriter::writeJson(FILE *fp, ParsedCsv csv)
{
    std::string root = makeTableName();
    std::vector<std::string> elementNames = makeElementNames(csv.getRecord(0));

    size_t firstDataRow = 0;
    fprintf(fp, "{\n");

    fprintf(fp, "\"%s\":\n", root.c_str());

    if (m_params.getHaveHeader())
    {
        firstDataRow = 1;
    }

    fprintf(fp, "[\n");

    bool even = true;
    size_t recNdx;
    for (recNdx = firstDataRow; recNdx < csv.numRecords() && !m_params.getFailed(); recNdx++)
    {
        size_t rowNum = recNdx - firstDataRow + 1;
        fprintf(fp, "  {\n");
        writeJsonRow(fp, elementNames, csv.getRecord(recNdx));
        fprintf(fp, "\n  }");

        if (recNdx < csv.numRecords() - 1)
            fprintf(fp, ",");

        fprintf(fp, "\n");
    }

    fprintf(fp, "]\n}\n");
}

void CsvJsonWriter::writeJsonRow(FILE *fp, 
                               std::vector<std::string> elementNames, 
                               CsvRecord csv)
{
    if (m_params.getHaveHeader())
    {
        if (csv.size() != elementNames.size())
        {
            m_params.setFailed(true);
            m_params.setErr("header and elements do not match when writing json\n"
                    "do you need -n?");
            return;
        }
    }

    size_t fieldNdx;
    for (fieldNdx = 0; fieldNdx < csv.size(); fieldNdx++)
    {
        char szElement[BUFSIZ];

        if (m_params.getHaveHeader())
            sprintf(szElement, "%s", elementNames[fieldNdx].c_str());
        else
            sprintf(szElement, "column%02ld", fieldNdx);


        std::string val = csv.getField(fieldNdx);

        splitstring::subst(val, "\\", "\\\\");
        splitstring::subst(val, "\"", "\\\"");
        splitstring::subst(val, "\n", "\\n");
        splitstring::subst(val, "\t", "\\t");
        splitstring::subst(val, "\r", "\\r");
        splitstring::subst(val, "\f", "\\f");
        splitstring::subst(val, "\b", "\\b");
        splitstring::subst(val, "/", "\\/");
        // uXXXX -> \uXXXX TODO

        if (val.empty())
            fprintf(fp, "    \"%s\": null", szElement);
        else
            fprintf(fp, "    \"%s\": \"%s\"", szElement, val.c_str());

        if (fieldNdx < csv.size() - 1)
            fprintf(fp, ",\n");
    }
}
