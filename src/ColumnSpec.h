// ColumnSpec.h
#ifndef have_ColumnSpec
#define have_ColumnSpec

#include <string>

class ColumnSpec
{
    std::string m_name;
    bool m_isInteger;
    bool m_isReal;
    int m_parameterIndex;

public:
    ColumnSpec()
        : m_isInteger(false),
          m_isReal(false)
    {
    };
   
    virtual ~ColumnSpec()
    {
    };

    int getParamIndex()
    {
        return m_parameterIndex;
    }

    void setParamIndex(int i)
    {
        m_parameterIndex = i;
    }

    std::string getName()
    {
        return m_name;
    }

    bool getIsInteger()
    {
        return m_isInteger;
    }

    bool getIsReal()
    {
        return m_isReal;
    }

    void setName(std::string s)
    {
        m_name = s;
    }

    void setIsIntegral(bool b)
    {
        m_isInteger = b;
    }

    void setIsReal(bool b)
    {
        m_isReal = b;
    }

    std::string getSqlSpec();
};



#endif // have_ColumnSpec

