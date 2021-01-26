// CsvRewriter.h
#ifndef have_CsvRewriter
#define have_CsvRewriter

#include <string>
#include "ParsedCsv.h" 
#include "split_string.h"
#include "ExprSkel.h"

enum OutputType
{
    CR_OT_HTML,
    CR_OT_XML,
    CR_OT_SQL,
    CR_OT_JSON,
    CR_OT_STATS,
    CR_OT_PIVOT,
    CR_OT_SYNTH,
    CR_OT_ASCII,
    CR_OT_TRANS
};

class CsvRewriteParams
{
    std::string m_err;
    std::string m_filename;
    OutputType m_outType;
    bool m_version;
    bool m_standalone;
    bool m_haveHeader;
    bool m_stripe;
    bool m_epsilon;
    bool m_css;
    std::string m_cssUrl;
    bool m_pivot;
    bool m_synth;
    std::string m_synName;
    std::string m_synthExpr;
    bool m_help;
    bool m_failed;

public:
    CsvRewriteParams()
        : m_outType(CR_OT_HTML)
        , m_version(false)
        , m_standalone(false)
        , m_stripe(true)
        , m_epsilon(false)
        , m_css(false)
        , m_pivot(false)
        , m_synth(false)
        , m_help(false)
        , m_failed(false)
    {}

    virtual ~CsvRewriteParams()
    {}

    bool getFailed()
    {
        return m_failed;
    }

    bool getHelp()
    {
        return m_help;
    }

    std::string getErr()
    {
        return m_err;
    }

    OutputType getOutType()
    {
        return m_outType;
    }

    bool getStandalone()
    {
        return m_standalone;
    }

    bool getVersion()
    {
        return m_version;
    }

    bool getHaveHeader()
    {
        return m_haveHeader;
    }

    bool getCss()
    {
        return m_css;
    }

    std::string getFilename()
    {
        return m_filename;
    }

    std::string getCssUrl()
    {
        return m_cssUrl;
    }

    std::string getSynthExpr()
    {
        return m_synthExpr;
    }

    std::string getSynName()
    {
        return m_synName;
    }

    bool getEpsilon()
    {
        return m_epsilon;
    }

    bool getStripe()
    {
        return m_stripe;
    }

    // sets
    void setFailed(bool b)
    {
        m_failed = b;
    }

    void setHelp(bool b)
    {
        m_help = b;
    }

    void setErr(std::string s)
    {
        m_err = s;
    }

    void setOutType(OutputType ot)
    {
        m_outType = ot;
    }

    void setVersion(bool b)
    {
        m_version = b;
    }

    void setStandalone(bool b)
    {
        m_standalone = b;
    }

    void setHaveHeader(bool b)
    {
        m_haveHeader = b;
    }

    void setStripe(bool b)
    {
        m_stripe = b;
    }

    void setEpsilon(bool b)
    {
        m_epsilon = b;
    }

    void setCss(bool b)
    {
        m_css = b;
    }

    void setCssUrl(std::string s)
    {
        m_cssUrl = s;
    }

    void setPivot(bool b)
    {
        m_pivot = b;
    }

    void setSynth(bool b)
    {
        m_synth = b;
    }

    void setSynName(std::string s)
    {
        m_synName = s;
    }

    void setSynthExpr(std::string s)
    {
        m_synthExpr = s;
    }

    void setFilename(std::string s)
    {
        m_filename = s;
    }
};

class CsvRewriter
{
    std::string subst(ExprSkel es, CsvRecord r);
public:
    CsvRewriter(CsvRewriteParams params);
    CsvRewriter(std::string rawCsv);
    virtual ~CsvRewriter();

    /*static*/ CsvRewriteParams m_params;

    static std::string m_strVersion;
    static std::string m_banner;
    ParsedCsv m_csv;
    FILE *m_ofp;

    //static void getopts(int argc, char *argv[]);

    std::vector<std::string> makeElementNames(CsvRecord csv);
    std::string makeTableName();

    void writeStats();
    void pivot();
    void synth();
    void ascii();
    void transpose();

    CsvRewriteParams getParams()
    {
        return m_params;
    }

    bool getFailed()
    {
        return m_params.getFailed();
    }

    bool getHelp()
    {
        return m_params.getHelp();
    }

    std::string getLastError()
    {
        return m_params.getErr();
    }

    std::string compressString(std::string s)
    {
        std::string res = s;
        splitstring::trimws(res);
        splitstring::fussy_trim(res, '\"');
        splitstring::subst(res, " ", "");

        return res;
    }
};



#endif // have_CsvRewriter

