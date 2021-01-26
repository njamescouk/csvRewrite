// ExprSkel.h
#ifndef have_ExprSkel
#define have_ExprSkel

struct StringOrIndex
{
    std::string m_str;
    size_t m_index;
    bool m_isString;
    bool m_isInt;

    StringOrIndex()
        : m_isInt(false)
        , m_isString(false)
    {}

    StringOrIndex(std::string s)
        : m_str(s)
        , m_isInt(false)
        , m_isString(true)
    {}

    StringOrIndex(size_t i)
        : m_index(i)
        , m_isString(false)
        , m_isInt(true)
    {}

    virtual ~StringOrIndex()
    {}
};

class ExprSkel : public std::vector<StringOrIndex>
{
    bool m_isValid;
    std::string m_err;

public:
    ExprSkel()
    : m_isValid(false)
    {}

    ExprSkel (std::string s)
    : m_isValid(false)
    {
        std::string curString;
        size_t i;
        for (i = 0; i < s.length(); /* NB */)
        {
            size_t curIndex;
            bool oneMoreChar = (i < s.length() - 1);
            if (s[i] == 'c')
            {
                if (oneMoreChar)
                {
                    if (!curString.empty())
                        push_back(curString);
                    curString.clear();
                    i++;
                    if (!isdigit(s[i]))
                    {
                        m_isValid = false;
                        m_err = "c followed by non digit: \"";
                        m_err += s.substr(i);
                        m_err += "\"";
                        // clear();
                        return;
                    }
                    size_t intEnd = s.substr(i).find_first_not_of("0123456789");
                    std::string intStr = s.substr(i, intEnd);
                    curIndex = atoi(intStr.c_str());
                    if (curIndex == 0)
                    {
                        m_isValid = false;
                        push_back(curString);
                        m_err = "error indexing starts at 1:\"";
                        m_err += s.substr(i);
                        m_err += "\"";
                        return;
                    }
                    StringOrIndex si(curIndex);
                    push_back(si);
                    if (intEnd == std::string::npos)
                        break;
                    i += intEnd;
                }
                else
                {
                    m_isValid = false;
                    push_back(curString);
                    m_err = s.substr(i);
                    // clear();
                    return;
                }
            }
            else
            {
                curString.push_back(s[i]);
                i++;
            }
        }

        if (!curString.empty())
        {
            StringOrIndex ss(curString);
            push_back(ss);
        }

        m_isValid = true;
    }

    virtual ~ExprSkel(){}

    bool isValid()
    {
        return m_isValid;
    }

    std::string getErr()
    {
        return m_err;
    }

    int maxIndex()
    {
        size_t res = 0;
        size_t i;
        for(i = 0; i < size(); i++)
        {
            if (at(i).m_isInt && at(i).m_index > res)
            {
                res = at(i).m_index;
            }
        }

        return (int)res;
    }

    std::string toString()
    {
        std::string res;

        size_t i;
        for(i = 0; i < size(); i++)
        {
            if (at(i).m_isInt)
            {
                char buf[BUFSIZ];
                sprintf(buf, "c%d", at(i).m_index);
                res += buf;
            }
            else if (at(i).m_isString)
            {
                res += at(i).m_str;
            }
        }

        return res;
    }
};



#endif // have_ExprSkel

