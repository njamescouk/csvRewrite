#include "CsvHtmlWriter.h" 

CsvHtmlWriter::CsvHtmlWriter(CsvRewriteParams params)
    : CsvRewriter(params)
{
}

CsvHtmlWriter::~CsvHtmlWriter() 
{
}

void CsvHtmlWriter::writeHtml(FILE *fp, ParsedCsv csv)
{
    if (m_params.getStandalone())
    {
        fprintf(fp, 
                "<!DOCTYPE html>\n<html>\n<head>\n"
                "  <meta name=\"generator\" content=\"CsvRewriter\" />\n");

        if (m_params.getCss())
            fprintf(fp, 
                    "  <link rel=\"stylesheet\" href=\"%s\" />\n", 
                    m_params.getCssUrl().c_str());
    
        fprintf(fp, "</head>\n<body>\n");
    }

    writeTable(fp, csv);

    if (m_params.getStandalone())
    {
        fprintf(fp, "</body>\n</html>\n");
    }

    fclose(fp);
}

void CsvHtmlWriter::writeTable(FILE *fp, ParsedCsv csv)
{
    // http://www.njames.co.uk/reference/csvRewrite.html#html-output

    bool even = true;
    
    ParsedCsv pc = csv;

    fprintf(fp, "<table>\n");

    size_t firstDataRow = 0;

    if(m_params.getHaveHeader())
    {
        writeHeader(fp, pc.getRecord(0));
        firstDataRow = 1;
    }

    fprintf(fp, "<tbody>\n");

    size_t recNdx;
    for (recNdx = firstDataRow; recNdx < pc.numRecords(); recNdx++)
    {
        CsvRecord curRec = pc.getRecord(recNdx);

        if (m_params.getStripe())
        {
            std::string parity = "even";
            if (!even)
                parity = "odd";
        
            fprintf(fp, "<tr class=\"%s\">\n", parity.c_str());
        }
        else
        {
            fprintf(fp, "<tr>\n");
        }

        size_t fieldNdx;
        for (fieldNdx = 0; fieldNdx < curRec.size(); fieldNdx++)
        {
            CsvField cf = curRec.getField(fieldNdx);
            std::string val(cf);
            if (cf.isQuoted())
                splitstring::subst(val, " ", "&nbsp;");
            splitstring::subst(val, "\n", "<br/>\n");

            if(val.empty() && m_params.getEpsilon())
                val = "&epsilon;";

            if (m_params.getStripe())
            {
                fprintf(fp, "<td class=\"column%02d\">%s</td>\n", fieldNdx+1, val.c_str());
            }
            else
            {
                fprintf(fp, "<td>%s</td>\n", val.c_str());
            }
        }

        fprintf(fp, "</tr>\n");
        even = !even;
    }

    fprintf(fp, "</tbody>\n");
    fprintf(fp, "</table>\n");
}

void CsvHtmlWriter::writeHeader(FILE *fp, CsvRecord csv)
{
    fprintf(fp, "<thead><tr>\n");

    size_t i;
    for (i = 0; i < csv.size(); i++)
    {
        fprintf(fp, "<th>%s</th>", csv.getField(i).c_str());
    }

    fprintf(fp, "</tr></thead>\n");
}
