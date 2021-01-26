// ExprEval.h
#ifndef have_ExprEval
#define have_ExprEval

#include <string>
#include <limits.h>
#include <float.h>
#include "eeYaccfns.h"

class ExprEval
{
    std::string m_scanString;
    bool m_valid;

public:
    ExprEval()
    {
        m_valid = false;
    }

    ExprEval(std::string s)
    {
        m_valid = true;
        m_scanString = s;
    }

    virtual ~ExprEval()
    {
    }

    bool getValid()
    {
        return m_valid;
    }

    double parse()
    {
        double res = DBL_MIN;

        eeScanString(m_scanString.c_str());
        if (eeparse() == 0)
        {
            res = g_result;
        }
        else
        {
            m_valid = false;
        }

        return res;
    }

};



#endif // have_ExprEval

