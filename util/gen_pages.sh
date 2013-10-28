#!/bin/sh
## run this script right from ./utils
(cd .. && scons docs)
git clone git@github.com:/basho/riak-c-client.git pages
cd pages
git checkout gh-pages
cp -r ../../docs/html/* ./
git add .
git commit -a -m "Generated gh_pages"
git push origin gh-pages

