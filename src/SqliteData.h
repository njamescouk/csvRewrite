// SqliteData.h
#ifndef have_SqliteData
#define have_SqliteData

#pragma warning (disable: 4996)

#include "sqlite3.h" // 94a0410aef9b50cd93016dcf35840abc954a7966 using getResultSet...
#include <cstring>
#include <cstdio>
#include <cstdlib>

/*
enum SU_DATA_TYPE
{
    SU_DATA_TYPE_NONE,
    SU_INTEGER,
    SU_FLOAT,
    SU_BLOB,
    SU_NULL,
    SU_TEXT
};

struct SuSqliteConv
{
    SU_DATA_TYPE suData(int sqliteType)
    {
        switch(sqliteType)
        {
        case SQLITE_INTEGER:
            return SU_INTEGER;
        case SQLITE_FLOAT:
            return SU_FLOAT;
        case SQLITE_BLOB:
            return SU_BLOB;
        case SQLITE_TEXT:
            return SU_TEXT;
        case SQLITE_NULL:
            return SU_NULL;
        }
    
        return SU_DATA_TYPE_NONE;
    }

    int sqliteData(SU_DATA_TYPE suType)
    {
        switch(suType)
        {
        case SU_INTEGER:
            return SQLITE_INTEGER;
        case SU_FLOAT:
            return SQLITE_FLOAT;
        case SU_BLOB:
            return SQLITE_BLOB;
        case SU_TEXT:
            return SQLITE_TEXT;
        case SU_NULL:
            return SQLITE_NULL;
        }
    
        return SQLITE_NULL;
    }
};
*/
class SqliteData
{
    int m_type;
    int m_parameterIndex;

    void *m_blob;
    int m_blobSize;
    double m_double;
    int m_int;
    std::string m_text;

public:
    SqliteData(int parameterIndex = -1)
        : m_type(SQLITE_NULL)
        , m_blobSize(0)
        , m_blob(0)
        , m_parameterIndex(parameterIndex)
    {
    }
    
    SqliteData(int parameterIndex, sqlite3_value *pv)
        : m_blobSize(0)
        , m_blob(0)
        , m_parameterIndex(parameterIndex)
    {
        sqlite3_value *pVal = sqlite3_value_dup(pv);
        m_type = sqlite3_value_type(pv);

        switch(m_type)
        {
        case SQLITE_INTEGER:
            m_int = sqlite3_value_int(pVal);
            break;
        case SQLITE_FLOAT:
            m_double = sqlite3_value_double(pVal);
            break;
        case SQLITE_BLOB:
            {
            m_blobSize = sqlite3_value_bytes(pVal);
            m_blob = calloc(m_blobSize, sizeof(unsigned char));
            const void *blobContent = sqlite3_value_blob(pVal);
            memcpy(m_blob, blobContent, m_blobSize);
            }
            break;
        case SQLITE3_TEXT:
            const unsigned char *pText = sqlite3_value_text(pVal);
            m_text = std::string((char *)pText);
            break;
        }

        sqlite3_value_free(pVal);
    }
    
    SqliteData(const SqliteData &d)
        : m_blobSize(0)
        , m_blob(0)
    {
        m_type = d.m_type;
        m_parameterIndex = d.m_parameterIndex;

        switch(m_type)
        {
        case SQLITE_INTEGER:
            m_int = d.m_int;
            break;
        case SQLITE_FLOAT:
            m_double = d.m_double;
            break;
        case SQLITE_BLOB:
            {
            free (m_blob);
            m_blob = calloc(m_blobSize, sizeof(unsigned char));
            memcpy(m_blob, d.m_blob, m_blobSize);
            }
            break;
        case SQLITE3_TEXT:
            m_text = d.m_text;
            break;
        }
    }
    
    SqliteData(int parameterIndex, double d)
        : m_type(SQLITE_FLOAT)
        , m_double(d)
        , m_blobSize(0)
        , m_blob(0)
        , m_parameterIndex(parameterIndex)
    {
    }
    
    SqliteData(int parameterIndex, int i)
        : m_type(SQLITE_INTEGER)
        , m_int(i)
        , m_blobSize(0)
        , m_blob(0)
        , m_parameterIndex(parameterIndex)
    {
    }
    
    SqliteData(int parameterIndex, std::string s)
        : m_type(SQLITE3_TEXT)
        , m_text(s)
        , m_blobSize(0)
        , m_blob(0)
        , m_parameterIndex(parameterIndex)
    {
    }
    
    virtual ~SqliteData()
    {
        if (m_type == SQLITE_BLOB)
            free (m_blob);
    }
    
    SqliteData & operator=(const SqliteData &rhs)
    {
        if (this == &rhs)      // Same object?
            return *this;        // Yes, so skip assignment, and just return *this.

        m_type = rhs.m_type;
        m_parameterIndex = rhs.m_parameterIndex;
    // ... // Deallocate, allocate new space, copy values...
        switch(m_type)
        {
        case SQLITE_INTEGER:
            m_int = rhs.m_int;
            break;
        case SQLITE_FLOAT:
            m_double = rhs.m_double;
            break;
        case SQLITE_BLOB:
            {
            free (m_blob);
            m_blob = calloc(m_blobSize, sizeof(unsigned char));
            memcpy(m_blob, rhs.m_blob, m_blobSize);
            }
            break;
        case SQLITE3_TEXT:
            m_text = rhs.m_text;
            break;
        }

        return *this;
    }

    int getType()
    {
        return m_type;
    }

    int getParameterIndex()
    {
        return m_parameterIndex;
    }

    int getInt()
    {
        if (m_type == SQLITE_INTEGER)
            return m_int;

        return 0;
    }

    double getDouble()
    {
        if (m_type == SQLITE_FLOAT)
            return m_double;

        return 0.0;
    }

    std::string getText()
    {
        if (m_type == SQLITE3_TEXT)
            return m_text;

        return std::string();
    }


    std::string toString()
    {
        std::string res;
        char buf[BUFSIZ];

        switch(m_type)
        {
        case SQLITE_INTEGER:
            _itoa(m_int, buf, 10);
            res = std::string(buf);
            break;
        case SQLITE_FLOAT:
            sprintf(buf, "%.5f", m_double);
            res = std::string(buf);
            break;
        case SQLITE_BLOB:
            res = "<blob>";
            break;
        case SQLITE3_TEXT:
            res = m_text;
            break;
        }
        return res;
    }
};



#endif // have_SqliteData

