// SqliteTable.h
#pragma once

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "sqliteRecord.h"
#include "split_string.h"

class SqliteTable
{
    bool m_valid;
    std::map<int, std::string> m_fieldNames; // parameterIndex -> field name
    std::vector <SqliteRecord> m_records;
    std::string m_name;
    std::string m_err;

public:

    SqliteTable()
    {
        m_valid = true;
    }

    SqliteTable(SqliteData d)
    {
        m_records.push_back(SqliteRecord(d));
        m_valid = true;
    }

    SqliteTable(SqliteRecord r)
    {
        m_records.push_back(r);
        m_valid = true;
    }
    /*
    SqliteTable(SqliteUtils *psu, std::string view)
    {
        m_valid = true;
        m_valid = m_valid && psu->isView(view);
        std::string query = "SELECT * FROM ";
        if (m_valid)
        {
            query += view + ";";
            m_valid = psu->getTable(query.c_str(), *this);
        }
    }
    */
    virtual ~SqliteTable(void)
    {
    }

    SqliteRecord operator[] (size_t i)
    {
        if (m_valid && i < m_records.size())
            return m_records[i];

        SqliteRecord r;
        return r;
    }

    void push_back(SqliteRecord r)
    {
        m_records.push_back(r);
    }

    void setColumnSpec(std::map<int, std::string> cs)
    {
        m_fieldNames = cs;
    }
    
    std::map<int, std::string> getColumnSpec()
    {
        return m_fieldNames;
    }

    size_t numFields()
    {
        if (m_records.size() > 0)
            return m_records[0].size();

        return 0;
    }

    size_t numFieldNames()
    {
        return m_fieldNames.size();
    }

    void setTableName (std::string s)
    {
        m_name = s;
    }

    std::string getTableName ()
    {
        return m_name;
    }

    int getType(int i)
    {
        if (numRows() > 0)
        {
            return m_records[0][i].getType();
        }

        return SQLITE_NULL;
    }

    std::vector <std::vector<std::string> > getRecords()
    {
        std::vector <std::vector<std::string> > *res = (std::vector <std::vector<std::string> > *)&m_records;

        return *res;
    }

    size_t numRows()
    {
        if (m_valid)
            return m_records.size();

        return 0;
    }

    bool isValid()
    {
        return m_valid;
    }

    void setValid(bool b)
    {
        m_valid = b;
    }

    void clear()
    {
        m_records.clear();
    }

    void addColumnSpec(int paramIndex, std::string fieldName)
    {
        m_fieldNames[paramIndex] = fieldName;
    }

    std::string getFieldName(size_t i)
    {
        if (m_valid && m_fieldNames.count(i) > 0)
            return m_fieldNames[i];

        return "";
    }

    /*
    */
    struct SubstSpace
    {
        char operator()(char c)
        {
                if (isspace(c)) 
                    return '_';

                return c;
        }
    };

    /*
        get m_fieldNames[i] and return a legal sqlite parameter for it
    */
    std::string getFieldParam(size_t i)
    {
        if (m_valid && m_fieldNames.count(i) > 0)
        {
            size_t pLen = m_fieldNames[i].size();
            std::string pName = m_fieldNames[i];
            std::transform(pName.begin(), 
                           pName.end(), 
                           pName.begin(), 
                           SubstSpace()
                           /*
                           [](char c)->char
                            {
                                if (isspace(c)) 
                                    return '_';
                                return c;
                            }*/// a bit javascripty...
                            ); 
            return ":" + pName + "Val";
        }

        return "";
    }

    std::string getLCaseFieldName(size_t i)
    {
        std::string res = getFieldName(i);
        res = splitstring::toLower(res);

        return res;
    }

    std::string makeColSpec()
    {
        std::string colSpec;
        size_t i;
        for (i = 0; i < m_fieldNames.size(); i++)
        {
            if (i != 0)
                colSpec += ",";

            colSpec += m_fieldNames[i];
        }

        return colSpec;
    }

    // return map (fieldName -> fieldValue) for rowIndex th record in *this
    std::map<std::string, SqliteData> makeRowMap(size_t rowIndex)
    {
        std::map<std::string, SqliteData> res;

        if (!(rowIndex > m_records.size()))
        {
            SqliteRecord r = m_records[rowIndex];
            size_t colNdx;
            for (colNdx = 0; colNdx < r.size() ; colNdx++)
            {
                SqliteData d = r[colNdx];
                std::string fieldNameOfSqliteData = m_fieldNames[d.getParameterIndex()];
                res[fieldNameOfSqliteData] = d;
            }
        }

        return res;
    }

    void printHtmlTable(FILE *fp, bool printHeader=true)
    {
        bool res = true;
        bool even = true;
    
        fprintf(fp, "<table>\n");

        if(printHeader)
        {
            printHtmlTableHeader(fp);
        }

        fprintf(fp, "<tbody>\n");

        std::vector<SqliteRecord>::iterator it;
        for (it = m_records.begin(); it != m_records.end(); it++)
        {
            SqliteRecord curRec = *it;

            /*
            if (m_stripe)
            {
                std::string parity = "even";
                if (!even)
                    parity = "odd";
        
                fprintf(fp, "<tr class=\"%s\">\n", parity.c_str());
            }
            else*/
            {
                fprintf(fp, "<tr>\n");
            }

            size_t fieldNdx;
            for (fieldNdx = 0; fieldNdx < curRec.size(); fieldNdx++)
            {
                std::string val = curRec[fieldNdx].toString();
                /// splitstring::subst(val, " ", "&nbsp;");
                if (val.find("</tr>") == std::string::npos)
                    splitstring::subst(val, "\n", "<br/>\n");
                
                /*
                if(val.empty() && m_epsilon)
                    val = "&epsilon;";

                if (m_stripe)
                {
                    fprintf(fp, "<td class=\"column%02d\">%s</td>\n", fieldNdx+1, val.c_str());
                }
                else*/
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

    void printHtmlTableHeader(FILE *fp)
    {
        fprintf(fp, "<thead><tr>\n");

        size_t i;
        for (i = 0; i < m_fieldNames.size(); i++)
        {
            fprintf(fp, "<th>%s</th>", m_fieldNames[i].c_str());
        }

        fprintf(fp, "</tr></thead>\n");
    }

    std::string sHtmlTable(std::string id="", bool printHeader=true)
    {
        std::string  res;
        bool even = true;
    
        if (id.empty())
        {
            res += "<table>\n";
        }
        else
        {
            res += "<table id=\"" + id + "\">\n";
        }

        if(printHeader)
        {
            res += sHtmlTableHeader();
        }

        res += "<tbody>\n";

        size_t recNdx;
        for (recNdx = 0; recNdx < m_records.size(); recNdx++)
        {
            SqliteRecord curRec = m_records[recNdx];

            std::string parity = "even";
            if (!even)
                parity = "odd";
        
            res += "<tr class=\"" + parity + "\">\n";

            size_t fieldNdx;
            for (fieldNdx = 0; fieldNdx < curRec.size(); fieldNdx++)
            {
                std::string val = curRec[fieldNdx].toString();
                splitstring::subst(val, "\n", "<br/>\n");
                size_t htttpPos = val.find("http://");
                size_t htttpsPos = val.find("https://");
                if (htttpPos < std::string::npos || htttpsPos < std::string::npos)
                {
                    size_t startPos = htttpPos > htttpsPos?htttpsPos:htttpPos;
                    size_t endPos = val.substr(startPos).find_first_of(" \t\r\n\f");
                    std::string addy = val.substr(startPos, endPos);
                    std::string link = "<a href=\"" + addy + "\">" + addy + "</a>";
                    std::string newVal = val.substr(0, startPos) + link;
                    if (endPos < std::string::npos)
                        newVal += val.substr(endPos);
                    res += "<td>" + newVal + "</td>\n";
                }
                else
                {
                    res += "<td>" + val + "</td>\n";
                }
            }

            res += "</tr>\n";
            even = !even;
        }

        res += "</tbody>\n"
               "</table>\n";

        return res;
    }

    std::string sDumbHtmlTable(std::string id="", bool printHeader=true)
    {
        std::string  res;
        bool even = true;
    
        if (id.empty())
        {
            res += "<table>\n";
        }
        else
        {
            res += "<table id=\"" + id + "\">\n";
        }

        if(printHeader)
        {
            res += sHtmlTableHeader();
        }

        res += "<tbody>\n";

        size_t recNdx;
        for (recNdx = 0; recNdx < m_records.size(); recNdx++)
        {
            SqliteRecord curRec = m_records[recNdx];

            res += "<tr>\n";

            size_t fieldNdx;
            for (fieldNdx = 0; fieldNdx < curRec.size(); fieldNdx++)
            {
                std::string val = curRec[fieldNdx].toString();
                res += "<td>" + val + "</td>\n";
            }

            res += "</tr>\n";
            even = !even;
        }

        res += "</tbody>\n"
               "</table>\n";

        return res;
    }

    std::string sHtmlTableHeader()
    {
        std::string  res;
        res += "<thead><tr>\n";

        size_t i;
        for (i = 0; i < m_fieldNames.size(); i++)
        {
            res += "<th>" + m_fieldNames[i] + "</th>";
        }

        res += "</tr></thead>\n";

        return res;
    }

    void printCsv(FILE *fp, bool printHeader=true)
    {
        bool res = true;
    
        if(printHeader)
        {
            printCsvTableHeader(fp);
            fputc('\n', fp);
        }

        size_t recNdx;
        for (recNdx = 0; recNdx < m_records.size(); recNdx++)
        {
            SqliteRecord curRec = m_records[recNdx];
            curRec.printCsv(fp);
        }
    }

    void printCsvTableHeader(FILE *fp)
    {
        size_t i;
        for (i = 0; i < m_fieldNames.size(); i++)
        {
            std::string s = m_fieldNames[i];
            if (s.find_first_of(" \t\r\n") != std::string::npos)
            {
                std::string qs = "\"" + s + "\"";
                s = qs;
            }

            if (i > 0)
                fprintf(fp, ",");
            fprintf(fp, "%s", s.c_str());
        }
    }


    std::string makeParamedInsert()
    {
        std::string res = "INSERT INTO \""
                        + m_name
                        + "\" (";

        size_t i;
        for (i = 0; i < numFields(); i++)
        {
            if (i > 0)
                res += ", ";

            res += "\"" + getFieldName(i) + "\"";
        }

        res += ") VALUES (";
        for (i = 0; i < numFields(); i++)
        {
            if (i > 0)
                res += ", ";

            res += getFieldParam(i);
        }

        res += ");";

        return res;
    }

    bool stringifyColumn(int colNdx, std::vector <std::string> &vs)
    {
        vs.clear();

        if (m_records.size() == 0)
            return true;

        if ((size_t)colNdx > m_fieldNames.size() || colNdx < 0)
            return false;

        size_t rowNdx;
        for (rowNdx = 0; rowNdx < m_records.size(); rowNdx++)
        {
            SqliteRecord r = m_records[rowNdx];
            SqliteData d = r[colNdx];
            vs.push_back(d.toString());
        }

        return true;
    }

    bool stringifyRow(int rowNdx, std::vector <std::string> &vs)
    {
        vs.clear();

        if (m_fieldNames.size() == 0)
            return true;

        if (m_fieldNames.size() == 0)
            return false;

        if ((size_t)rowNdx > m_fieldNames.size() || rowNdx < 0)
            return false;

        SqliteRecord r = m_records[rowNdx];

        size_t colNdx;
        for (colNdx = 0; colNdx < r.size(); colNdx++)
        {
            SqliteData d = r[colNdx];
            vs.push_back(d.toString());
        }

        return true;
    }

    bool stringifyValue(int rowNdx, int colNdx, std::string &s)
    {
        s.clear();

        if (rowNdx < 0 
            || colNdx < 0 
            || (size_t)rowNdx > m_records.size() 
            || (size_t)colNdx > m_fieldNames.size()
            || m_records.size() == 0
            || m_fieldNames.size() == 0)
            return false;

        s = m_records[rowNdx][colNdx].toString();

        return true;
    }

};
