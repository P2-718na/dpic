#!/usr/bin/env bash
#incomplete build without bibtex
mkdir -p ./docs/report/build/sections &&
cd docs/report &&
texfot xelatex -file-line-error -interaction=nonstopmode  -output-directory=build ./report.tex;