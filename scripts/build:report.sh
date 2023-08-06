#!/usr/bin/env bash
mkdir -p ./docs/report/build &&
cd docs/report &&
export "$(xargs < .env)" &&
pdflatex -output-directory=build ./report.tex;
bibtex build/report.aux;     # Whoever wrote bibtex needs to just stop.
pdflatex -output-directory=build ./report.tex && # Yep. Needed for references.
pdflatex -output-directory=build ./report.tex;   # Yep. Needed for bibtex.
