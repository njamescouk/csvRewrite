#include "CsvXmlWriter.h" 

CsvXmlWriter::CsvXmlWriter(CsvRewriteParams params)
    : CsvRewriter(params)
{
}

CsvXmlWriter::~CsvXmlWriter() 
{
}

void CsvXmlWriter::writeXml(FILE *fp, ParsedCsv csv)
{
    std::string doctype = makeTableName();
    std::vector<std::string> elementNames = makeXmlElementNames(csv.getRecord(0));

    size_t firstDataRow = 0;
    if (m_params.getStandalone())
    {
        fprintf(fp, "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n");
        if (csv.isRectangular())
        {
            writeDoctype(fp, doctype, elementNames, csv.getRecord(0));
        }
        /*
        else
        {
            m_params.setFailed(true);
            m_params.setErr("cannot write DOCTYPE for non rectangular csv\n"
                             "if you're using -s, try without it");
            return;
        }*/

        if (m_params.getHaveHeader())
        {
            firstDataRow = 1;
        }

        if (m_params.getCss())
        {
            size_t lastDotPos = m_params.getCssUrl().find_last_of(".");
            if (lastDotPos == std::string::npos)
            {
                m_params.setFailed(true);
                m_params.setErr("can't find extension for style sheet file name");
                return;
            }

            std::string ext = m_params.getCssUrl().substr(lastDotPos + 1, std::string::npos);
            if (ext == "css" | ext == "xsl")
            {
                fprintf(fp, 
                        "<?xml-stylesheet type=\"text/%s\" href=\"%s\" ?>\n", 
                        ext.c_str(), 
                        m_params.getCssUrl().c_str());
            }
            else
            {
                m_params.setFailed(true);
                m_params.setErr("style sheet file name should be css or xsl");
                return;
            }
        }
    }

    fprintf(fp, "\n");
    fprintf(fp, "<%s>\n", doctype.c_str());

    bool even = true;
    size_t recNdx;
    for (recNdx = firstDataRow; recNdx < csv.numRecords() && !getFailed(); recNdx++)
    {
        size_t rowNum = recNdx - firstDataRow + 1;
        if (m_params.getStripe())
        {
            std::string parity = "even";
            if (rowNum % 2 == 1)
                parity = "odd";
        
            fprintf(fp, "<row number=\"%ld\" parity=\"%s\">\n", rowNum, parity.c_str());
        }
        else
        {
            fprintf(fp, "<row number=\"%ld\">\n", rowNum);
        }

        writeXmlRow(fp, elementNames, csv.getRecord(recNdx));
        fprintf(fp, "</row>\n");
    }

    fprintf(fp, "</%s>\n", doctype.c_str());

}

std::vector<std::string> CsvXmlWriter::makeXmlElementNames(CsvRecord csv)
{
    std::vector<std::string> res;

    size_t i;
    for (i = 0; i < csv.size(); i++)
    {
        if (m_params.getHaveHeader())
        {
            res.push_back(compressString(csv.getField(i)));
        }
    }

    return res;
}

void CsvXmlWriter::writeDoctype(FILE *fp, 
                               std::string doctype, 
                               std::vector<std::string> elementNames, 
                               CsvRecord csv)
{
    if (m_params.getHaveHeader())
    {
        if (elementNames.size() != csv.size())
        {
            m_params.setErr("element names and csv records don't match\n"
                            "do you need -n?");
            m_params.setFailed(true);
            return;
        }
    }

    fprintf(fp, "<!DOCTYPE %s [\n", doctype.c_str());

    // write root element defn
    fprintf(fp, "<!ELEMENT %s (row)*>\n", doctype.c_str());

    fprintf(fp, "<!ELEMENT row (", doctype.c_str());

    size_t i;
    for (i = 0; i < csv.size(); i++)
    {
        if (m_params.getHaveHeader())
            fprintf(fp, "%s?", elementNames[i].c_str());
        else
            fprintf(fp, "column%02ld?", i);

        if (i < csv.size() - 1)
            fprintf(fp, ",");
    }
    fprintf(fp, ")*>\n");

    fprintf(fp, "<!ATTLIST row\n");
    fprintf(fp, 
             "      number CDATA #REQUIRED\n"
             "      parity CDATA #IMPLIED>\n");

    // write element defns
    for (i = 0; i < csv.size(); i++)
    {
        if (m_params.getHaveHeader())
            fprintf(fp, "<!ELEMENT %s (#PCDATA)>\n", elementNames[i].c_str());
        else
            fprintf(fp, "<!ELEMENT column%02ld (#PCDATA)>\n", i);
    }

    fprintf(fp, "]>\n");
}

void CsvXmlWriter::writeXmlRow(FILE *fp, 
                            std::vector<std::string> elementNames, 
                            CsvRecord csv)
{
    if (m_params.getHaveHeader())
    {
        if (csv.size() != elementNames.size())
        {
            m_params.setFailed(true);
            m_params.setErr("header and elements do not match when writing xml\n"
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
        /// splitstring::subst(val, " ", "&nbsp;");
        // splitstring::subst(val, "\n", "&#10;\n");


        if(val.empty() && m_params.getEpsilon())
            val = "&#949;";

        // if (!val.empty())
            // fprintf(fp, "<%s><![CDATA[%s]]></%s>", element.c_str(), val.c_str(), element.c_str());
            fprintf(fp, "    <%s>%s</%s>\n", szElement, val.c_str(), szElement);
    }

   //  even = !even;
}
