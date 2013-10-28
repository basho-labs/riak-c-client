#!/bin/sh
cd .. && scons docs
mkdir ../riakc_pages
cp -r ./docs/html/* ../riakc_pages

