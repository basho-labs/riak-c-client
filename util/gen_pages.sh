#!/bin/sh
## run this script right from ./utils
cd .. && scons docs
mkdir ../riakc_pages
cp -r ./docs/html/* ../riakc_pages
git checkout gh-pages
cp -r ../riakc_pages/* ./
git commit -a -m "Generated gh_pages"
git push origin gh_pages

