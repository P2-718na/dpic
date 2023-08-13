#!/usr/bin/env bash
mkdir -p ./docs/report/build &&
cd docs/report &&
#export "$(xargs < .env)" &&
xelatex -interaction=batchmode  -output-directory=build ./report.tex > /dev/null;
makeglossaries -d build/sections sections/glossary;
texfot bibtex build/report.aux;     # Whoever wrote bibtex needs to just stop.
xelatex -interaction=batchmode -output-directory=build ./report.tex  > /dev/null; # Yep. Needed for references.
texfot xelatex -file-line-error -interaction=nonstopmode  -output-directory=build ./report.tex;   # Yep. Needed for bibtex.
