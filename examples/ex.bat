@echo off
echo ex01.csv fragments
for %%t in (html json ascii sql xml) do echo rewrite -t %%t ex01.csv & ..\bin\csvRewrite -eps -n -t %%t ex01.csv > ex01.csv.fragment.%%t
echo.
echo ex01.csv standalone
for %%t in (html json ascii sql xml) do echo rewrite -t %%t ex01.csv & ..\bin\csvRewrite -eps -n -s -t %%t ex01.csv > ex01.csv.%%t
echo.
echo ex02.csv fragments
for %%t in (html json ascii sql xml) do echo rewrite -t %%t ex02.csv & ..\bin\csvRewrite -eps -n -t %%t ex02.csv > ex02.csv.fragment.%%t
echo.
echo ex02.csv standalone
for %%t in (html json ascii sql xml) do echo rewrite -t %%t ex02.csv & ..\bin\csvRewrite -eps -n -s -t %%t ex02.csv > ex02.csv.%%t

echo.
echo transaction-volumes.csv standalone
for %%t in (html json ascii sql xml) do echo rewrite -t %%t ex-transaction-volumes.csv & ..\bin\csvRewrite -eps -s -t %%t -css exXml.css ex-transaction-volumes.csv > transaction-volumes.csv.%%t

echo.
echo running sql - no news is good news - you need sqlite3 for this...
for %%t in (*.sql) do echo %%t & sqlite3 < %%t


echo.
echo -t xml with css - view in browser
echo ..\bin\csvRewrite -css exXml.css -eps -n -s -t xml ex01.csv & ..\bin\csvRewrite -css exXml.css -eps -n -s -t xml ex01.csv > ex01.csv.css.xml
echo ..\bin\csvRewrite -css exXml.css -eps -n -s -t xml ex02.csv && ..\bin\csvRewrite -css exXml.css -eps -n -s -t xml ex02.csv > ex02.csv.css.xml

echo.
echo ..\bin\csvRewrite -synth "2 * c1 * (22.0/7.0)" -synthName area ex03.csv
..\bin\csvRewrite -synth "2 * c1 * (22.0/7.0)" -synName area ex03.csv > ex04.csv
