#include "CsvRewriter.h" 
#include "CsvHtmlWriter.h"
#include "CsvXmlWriter.h"
#include "CsvJsonWriter.h"
#include "CsvSqlWriter.h"
#include "OptionSpec.h"
#include "CmdLineSpec.h"
#include "CmdLineParser.h"

#pragma warning (disable: 4996)

char *progname = 0;
/*
extern FILE *yyin;
int yylex (void);
*/
void rewrite(CsvRewriteParams &params);
CsvRewriteParams getopts(int argc, char *argv[]);

// -t sql I:\usr\NIK\dev\filterize\example\ReadingListonPhilosophyofScience.csv
// -t sql I:\usr\NIK\DOM\robbingBastards\meterbase\sql\gas.csv
int main (int argc, char *argv[])
{
    progname = argv[0];

    CsvRewriteParams params = getopts(argc, argv);

    if (params.getFailed())
    {
        fprintf(stderr, "csvRewrite failed: %s\n", params.getErr().c_str());
        return 2;
    }

    if (params.getHelp())
    {
        return 0;
    }

    rewrite(params);

    if (params.getFailed())
    {
        fprintf(stderr, "csvRewrite failed: %s\n", params.getErr().c_str());
        return __LINE__;
    }

    return 0;
}

void rewrite(CsvRewriteParams &params)
{
    switch(params.getOutType())
    {
        case CR_OT_HTML:
        {
            CsvHtmlWriter chw(params);
            chw.write();
            break;
        }
        case CR_OT_XML:
        {
            CsvXmlWriter cxw(params);
            cxw.write();
            break;
        }
        case CR_OT_JSON:
        {
            CsvJsonWriter cjw(params);
            cjw.write();
            break;
        }
        case CR_OT_SQL:
        {
            CsvSqlWriter csw(params);
            csw.writeSql(stdout);
            break;
        }
        case CR_OT_STATS:
        {
            CsvRewriter cr(params);
            cr.writeStats();
            break;
        }
        case CR_OT_PIVOT:
        {
            CsvRewriter cr(params);
            cr.pivot();
            break;
        }
        case CR_OT_SYNTH:
        {
            CsvRewriter cr(params);
            cr.synth();
            params = cr.getParams();
            break;
        }
        case CR_OT_ASCII:
        {
            CsvRewriter cr(params);
            cr.ascii();
            break;
        }
        case CR_OT_TRANS:
        {
            CsvRewriter cr(params);
            cr.transpose();
            break;
        }
    }
}

CsvRewriteParams getopts(int argc, char *argv[])
{
    CsvRewriteParams res;

    res.setFailed(false);
    res.setOutType(CR_OT_HTML);
    res.setHaveHeader(true);

    // see http://www.njames.co.uk/reference/csvRewrite.html#synopsis

    std::map<std::string, OptionSpec> specs;

    specs["h"]     = OptionSpec(false, "help");
    specs["v"]     = OptionSpec(false, "to write version to stdout.");
    specs["s"]     = OptionSpec(false, "produces a standalone file, otherwise a fragment is produced");
    specs["n"]     = OptionSpec(false, "no header on the first line.");
    specs["t"]     = OptionSpec(true,  "type. Specifies the output; possible values are\n"
                                       "                        html (the default), xml, json, sql.");
    specs["z"]     = OptionSpec(false, "zebra. to enable striped tables via <tr class=\"even|odd\">\n"
                                       "                        and user css.");
    specs["eps"]   = OptionSpec(false, "to write &epsilon; in empty fields");
    specs["css"]   = OptionSpec(true,  "to write a css reference to url in the html <head>.");
    specs["stats"] = OptionSpec(false, "to write summary data for filename to stdout.");
    specs["pivot"] = OptionSpec(false, "to turn a,1,2 into a,1\\na,2 and so on.\n"
                                       "                        emits csv and ignores -t");
    specs["synth"] = OptionSpec(true,  "synthesise a new column by simple arithmetic\n"
                                       "                        operations on columns eg \"c1*c4-c3\" copies the table\n"
                                       "                        and produces a new columnN with computed values.\n"
                                       "                        emits csv and ignores -t");
    specs["synName"]=OptionSpec(true,  "name for synthesised column");
    specs["trans"] = OptionSpec(false, "transpose. turn a,1,2 into a\\n1\\n2\\n and so on.\n"
                                       "                        emits csv and ignores -t");
    specs["ascii"] = OptionSpec(false, "use 0x1e record separator and 0x1f unit separator\n"
                                       "                        to delimit fields");

    CmdLineSpec cls(specs);

    CmdLineParser clp(argc, argv, cls);
    Cmdline cl = clp.parse();
    if (!cl.isValid())
    {
        res.setFailed(true);
        res.setErr("failed to parse command line");
    }
    else
    {
        if (cl.hasOption("t"))
        {
            std::string tOpt = cl.getOptionValue("t");

            if (tOpt == "xml")
            {
                res.setOutType(CR_OT_XML);
            }
            else if (tOpt == "html")
            {
                res.setOutType(CR_OT_HTML);
            }
            else if (tOpt == "sql")
            {
                res.setOutType(CR_OT_SQL);
            }
            else if (tOpt == "json")
            {
                res.setOutType(CR_OT_JSON);
            }
            else if (tOpt == "ascii")
            {
                res.setOutType(CR_OT_ASCII);
            }
            else
            {
                res.setErr("the only output types supported are html, xml, json sql and ascii");
                res.setFailed(true);
            }
        }

        if (cl.hasOption("h"))
        {
            res.setHelp(true);
            std::string blurb =  "csvRewrite [-v] [-stats] [-s] [-n] [-t type] [-z] [-eps] [-css url]\n"
                                 "[-pivot] [-synth expr] [-synName columnName] [filename]\n"
                                 "rewrites csv in filename as type (default html or xml or sql or json\n"
                                 "or ascii) to stdout. If no filename is given stdin is read.\n";
            cls.help(blurb, stdout);
        }

        res.setVersion(cl.hasOption("v"));
        res.setStandalone(cl.hasOption("s"));
        res.setHaveHeader(!cl.hasOption("n"));
        res.setStripe(cl.hasOption("z"));
        res.setEpsilon(cl.hasOption("eps"));
        res.setHelp(cl.hasOption("h"));

        if (cl.hasOption("css"))
        {
            res.setCss(true);
            res.setCssUrl(cl.getOptionValue("css"));
        }
        else
        {
            res.setCss(false);
        }

        if (cl.hasOption("stats"))
        {
            res.setOutType(CR_OT_STATS);
        }
        else if (cl.hasOption("pivot"))
        {
            res.setPivot(true);
            res.setOutType(CR_OT_PIVOT);
        }
        else if (cl.hasOption("synth"))
        {
            res.setSynth(true);
            res.setOutType(CR_OT_SYNTH);
            res.setSynthExpr(cl.getOptionValue("synth"));

            if (cl.hasOption("synName"))
                res.setSynName(cl.getOptionValue("synName"));
        }
        else if (cl.hasOption("trans"))
        {
            res.setOutType(CR_OT_TRANS);
        }

        if (cl.numArguments() > 0)
        {
            res.setFilename(cl.getArgument(0));
        }
    }

    return res;
}
