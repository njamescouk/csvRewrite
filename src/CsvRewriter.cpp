#include <string>
#include <cstring>
#include <math.h>
#include "CsvRewriter.h" 
#include "CsvParser.h" 
#include "ExprEval.h" 
#include "ExprSkel.h"

#pragma warning(disable : 4996)

std::string CsvRewriter::m_strVersion =  "1.5";
std::string CsvRewriter::m_banner = "csvRewrite";
//CsvRewriteParams CsvRewriter::m_params;

// TODO write csv as tex table
/*
write to string then FILE
*/
CsvRewriter::CsvRewriter(CsvRewriteParams params) 
    : m_params(params)
{
    m_params.setHelp(false);
    m_params.setFailed(false);
    m_ofp = stdout;

    if (m_params.getVersion())
    {
        fprintf (stdout, "%s version %s\n", m_banner.c_str(), m_strVersion.c_str());
        return;
    }

    if (m_params.getHelp())
    {
        return;
    }

    CsvParser cp;
    bool parsed = false;
    if (m_params.getFilename().empty())
        parsed = cp.parse(stdin);
    else
        parsed = cp.parse(m_params.getFilename());

    if (!parsed)
    {
        m_params.setFailed(true);
        m_params.setErr("failed to parse " + m_params.getFilename());
    }
    else if (cp.getParsedCsv().numRecords() == 0)
    {
        m_params.setFailed(true);
        m_params.setErr("no records found in " + m_params.getFilename());

        return;
    }
    else if (m_params.getHaveHeader() && cp.getParsedCsv().numRecords() == 1)
    {
        m_params.setFailed(true);
        m_params.setErr("header only in " + m_params.getFilename());

        return;
    }
    else
    {
        m_csv = cp.getParsedCsv();
        m_csv.applyColumnTypes(m_params.getHaveHeader());
    }
}

CsvRewriter::CsvRewriter(std::string rawCsv)
{
    CsvParser cp;
    bool parsed = cp.parseString(rawCsv.c_str());
    if (!parsed)
    {
        m_params.setFailed(true);
        m_params.setErr("failed to parse raw csv");
    }
    else if (cp.getParsedCsv().numRecords() == 0)
    {
        m_params.setFailed(true);
        m_params.setErr("no records found in raw csv");

        return;
    }
    else
    {
        m_csv = cp.getParsedCsv();
        m_csv.applyColumnTypes(true);
    }
}

CsvRewriter::~CsvRewriter() 
{
}

std::string CsvRewriter::makeTableName()
{
    std::string tableName = m_params.getFilename();
    size_t lastSlashPos = m_params.getFilename().find_last_of("\\/");
    if (lastSlashPos != std::string::npos && lastSlashPos < m_params.getFilename().length()-1)
        tableName = m_params.getFilename().substr(lastSlashPos + 1);

    size_t lastDotPos = tableName.find_last_of(".");
    if (lastDotPos != std::string::npos && lastDotPos < tableName.length()-1)
        tableName = tableName.substr(0, lastDotPos);

    splitstring::subst(tableName, " ", "");

    return tableName;
}

std::vector<std::string> CsvRewriter::makeElementNames(CsvRecord csv)
{
    std::vector<std::string> res;

    size_t i;
    for (i = 0; i < csv.size(); i++)
    {
        if (m_params.getHaveHeader())
        {
            res.push_back(/*compressString*/(csv.getField(i)));
        }
    }

    return res;
}

void CsvRewriter::writeStats()
{
    fprintf(m_ofp, "%s \n", m_csv.isRectangular() ? "rectangular" : "not rectangular");
    fprintf(m_ofp, "%ld records\n", m_csv.numRecords());
    fprintf(m_ofp, "max fields %ld\n", m_csv.maxFields());
    fprintf(m_ofp, "min fields %ld\n", m_csv.minFields());
}

void CsvRewriter::pivot()
{
    size_t recNdx;
    for (recNdx = 0; recNdx < m_csv.numRecords(); recNdx++)
    {
        CsvRecord curRec = m_csv.getRecord(recNdx);
        if (curRec.size() == 1)
        {
            fprintf(m_ofp, "%s\n", curRec[0].c_str());
        }
        else if (curRec.size() > 1)
        {
            std::string firstField = curRec[0];
            size_t fldNdx;
            for (fldNdx = 1/*NB*/; fldNdx < curRec.size(); fldNdx++)
            {
                CsvField curFld = curRec[fldNdx];
                if (!curFld.empty())
                    fprintf(m_ofp, "%s,%s\n", firstField.c_str(), curFld.c_str());
            }
        }
    }
}

void CsvRewriter::synth()
{
    if (m_csv.minFields() != m_csv.maxFields())
    {
        m_params.setErr("cannot synthesise, csv not rectangular");
        m_params.setFailed(true);
        return;
    }

    ExprSkel es(m_params.getSynthExpr());

    if (!es.isValid())
    {
        std::string msg = "expr error\n";
                    msg += es.toString();
                    msg += "\"";
                    msg += es.getErr();
                    msg += "\"";
        m_params.setErr(msg);
        m_params.setFailed(true);
        return;
    }

    if (es.maxIndex() > (int)m_csv.maxFields())
    {
        char msg[BUFSIZ];
        sprintf(msg, 
                "expr error\nindex %d but only %d columns", 
                es.maxIndex(),
                (int)m_csv.maxFields());
        m_params.setErr(msg);
        m_params.setFailed(true);
        return;
    }

    int newColNum = (int)m_csv.maxFields() + 1;
    std::vector<CsvRecord> newCsv;
    newCsv.reserve(m_csv.numRecords());
    size_t curRow = 0;
    if (m_params.getHaveHeader())
    {
        char newFieldName[BUFSIZ];
        if (m_params.getSynName().empty())
        {
            sprintf(newFieldName, "column%d", newColNum);
        }
        else
        {
            strcpy(newFieldName, m_params.getSynName().c_str());
        }
        CsvRecord newHead = m_csv.getRecord(0);
        CsvField newField(newFieldName, true, CSV_FIELD_TYPE_TEXT);
        newHead.push_back(newField);
        newCsv.push_back(newHead);
        curRow++;
    }

    for (/* NB */; curRow < m_csv.numRecords(); curRow++)
    {
        CsvRecord curRec = m_csv.getRecord(curRow);
        std::string sExpr = subst(es, curRec);
        ExprEval ee(sExpr);
        double d = ee.parse();
        CsvField newFld;
        if (ee.getValid())
        {
            if (fabs(d - floor(d)) < .00000000001)
            {
                char buf[BUFSIZ];
                sprintf(buf, "%d", (int)floor(d));
                newFld = CsvField(buf, false, CSV_FIELD_TYPE_INTEGRAL);
            }
            else
            {
                newFld = CsvField(d);
            }
        }
        else
        {
            newFld = CsvField("ERR", true, CSV_FIELD_TYPE_TEXT);
        }
        CsvRecord newRec = curRec;
        newRec.push_back(newFld);
        newCsv.push_back(newRec);
    }

    size_t recNdx;
    for (recNdx = 0; recNdx < newCsv.size(); recNdx++)
    {
        CsvRecord curRec = newCsv[recNdx];
        size_t fldNdx;
        for (fldNdx = 0; fldNdx < curRec.size(); fldNdx++)
        {
            if (fldNdx > 0)
                fputc(',', m_ofp);
            curRec[fldNdx].write(m_ofp);
        }
        fputc('\n', m_ofp);
    }
}

void CsvRewriter::transpose()
{
    if (m_csv.minFields() != m_csv.maxFields())
    {
        m_params.setErr("cannot transpose, csv not rectangular");
        m_params.setFailed(true);
        return;
    }

    size_t numTransposedColumns = m_csv.numRecords();
    size_t recNdx;
    for (recNdx = 0; recNdx < m_csv.minFields(); recNdx++)
    {
        size_t transposedColNdx;
        for (transposedColNdx = 0; transposedColNdx < numTransposedColumns; transposedColNdx++)
        {
            CsvRecord curRec = m_csv.getRecord(transposedColNdx);
            CsvField curField = curRec.getField(recNdx);
            if (transposedColNdx > 0)
                fprintf(m_ofp, ",");
            curField.write(m_ofp);
        }
        fprintf(m_ofp, "\n");
    }
}

void CsvRewriter::ascii()
{
    size_t recNdx;
    for (recNdx = 0; recNdx < m_csv.numRecords(); recNdx++)
    {
        putc(0x1e, m_ofp);
        CsvRecord curRec = m_csv.getRecord(recNdx);
        size_t fldNdx;
        for (fldNdx = 0; fldNdx < curRec.size(); fldNdx++)
        {
            putc(0x1f, m_ofp);
            fprintf(m_ofp, "%s", curRec[fldNdx].c_str());
        }
    }
}

std::string CsvRewriter::subst(ExprSkel es, CsvRecord r)
{
    std::string res;

    size_t i;
    for(i = 0; i < es.size(); i++)
    {
        if (es[i].m_isString)
        {
            res += es[i].m_str;
        }
        else if (es[i].m_isInt)
        {
            int recordNdx = es[i].m_index-1;
            double d = r[recordNdx].getDouble();
            if (d == -DBL_MAX)
            {
                res += "ERR";
            }
            else
            {
                char buf[BUFSIZ];
                sprintf(buf, "%f", d);
                res += buf;
            }
        }
    }

    return res;
}
