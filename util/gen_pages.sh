#!/bin/sh
cwd=${PWD##*/}
if [ "$cwd" != "util" ]
then
  echo "Please run gen_pages.sh from the ./util directory"
  exit 1
fi

## run this script right from ./utils
(cd .. && ./autogen.sh && doxygen)
rm -rf ./pages
git clone git@github.com:/basho/riak-c-client.git pages
cd pages
git checkout gh-pages
cp -r ../../docs/html/* ./
git add .
git commit -a -m "Generated gh_pages"
git push origin gh-pages

