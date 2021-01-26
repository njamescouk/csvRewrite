// CsvSqlWriter.h
#ifndef have_CsvSqlWriter
#define have_CsvSqlWriter

#include <cstdio>
#include "CsvRewriter.h"
#include "ColumnSpec.h"
class SqliteTable;

#define CSV_ORDER_TABLE_NAME "csvOrder"

class CsvSqlWriter : public CsvRewriter 
{
    std::vector<ColumnSpec> m_colSpec;
    std::string m_tableName;

    std::vector<ColumnSpec> makeSqlColSpec();
    void writeCreateTable(FILE *fp);

    std::string sWriteCreateTable();
    void writeColSpec(FILE *fp, bool allOneLine=true);
    std::string sWriteColSpec(bool allOneLine=true);
    std::string makeSqlColumnList();
    std::string makeSqlValueList(CsvRecord rec);

public:
    CsvSqlWriter(CsvRewriteParams params);
    CsvSqlWriter(std::string rawCsv);
    virtual ~CsvSqlWriter();

    std::string getTableName()
    {
        return m_tableName;
    }

    void writeSql(FILE *fp);
    std::string sWriteCsvOrderTableCreate();
    std::string sWriteCsvOrderTableInsert();
    bool sWriteCreates(std::string &sql);
    bool sWriteInserts(std::string &sql);
    bool toTable(SqliteTable &t);
};



#endif // have_CsvSqlWriter

