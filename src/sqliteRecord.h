#pragma once

#include <string>
#include <vector>
#include <cstdio>
#include "SqliteData.h"

class SqliteRecord : public std::vector<SqliteData>
{
public:
    SqliteRecord ()
    {}

    SqliteRecord(SqliteData d)
    {
        push_back(d);
    }

    SqliteRecord (std::vector<std::string> vs)
    {
        size_t i;

        for (i = 0; i < vs.size(); i++)
        {
            push_back(SqliteData(i, vs[i]));
        }
    }

    virtual ~SqliteRecord ()
    {}

    SqliteData getDataForParam(size_t paramNdx)
    {
        size_t i;
        for (i = 0; i < size(); i++)
        {
            if (at(i).getParameterIndex() == (int)paramNdx)
                return at(i);
        }

        return SqliteData (paramNdx);
    }

    void printCsv(FILE *fp)
    {
        size_t i;
        for (i = 0; i < size(); i++)
        {
            if (i > 0)
                fprintf(fp, ",");

            std::string s = at(i).toString();
            if (s.find_first_of(" \t\r\n") != std::string::npos)
            {
                std::string qs = "\"" + s + "\"";
                s = qs;
            }

            fprintf(fp, "%s", s.c_str());
        }
        fprintf(fp, "\n");
    }

};

