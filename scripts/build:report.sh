#!/usr/bin/env bash
for i in {1..6}
do
  mkdir -p ./docs/report/build/sections/$i
done;
cd docs/report
#export "$(xargs < .env)" &&
pdflatex -interaction=batchmode  -output-directory=build ./report.tex > /dev/null;
#makeglossaries -d build/sections sections/glossary;
texfot biber  --input-directory  build --output-director build report;     # Whoever wrote bibtex needs to just stop.
pdflatex -interaction=batchmode -output-directory=build ./report.tex  > /dev/null; # Yep. Needed for references.
texfot pdflatex -file-line-error -interaction=nonstopmode  -output-directory=build ./report.tex &&   # Yep. Needed for bibtex.
cp build/report.pdf build/report-stable.pdf;

echo "DONE";