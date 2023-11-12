#!/usr/bin/env bash
#incomplete build without bibtex
for i in {1..6}
do
  mkdir -p ./docs/report/build/sections/$i
done;
cd docs/report &&
texfot xelatex -file-line-error -interaction=nonstopmode  -output-directory=build ./report.tex &&
cp build/report.pdf build/report-stable.pdf;