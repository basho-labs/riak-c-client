#!/bin/sh
## run this script right from ./utils
(cd .. && scons docs)
   #cp -r ../docs/html/* ../riakc_pages
git clone git@github.com:/basho/riak-c-client.git pages
cd pages
git checkout gh-pages
cp -r ../../riakc_pages/* ./
git add .
git commit -a -m "Generated gh_pages"
git push origin gh_pages

