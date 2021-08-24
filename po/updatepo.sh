#!/usr/bin/env bash

INPUT=`find ../src -type f -name *.cpp`

xgettext -C --from-code=UTF-8 --msgid-bugs-address=archdvx@dxsolutions.org --package-name=wxdfast\
    --package-version=0.70.1 --copyright-holder="David Vachulka" --keyword=_ --keyword=N_ --keyword=_n:1,2\
    --keyword=wxPLURAL:1,2 -o wxdfast.pot $INPUT

for i in *.po
do
  echo "Updating $i ..."
  msgmerge --update --backup=off $i wxdfast.pot
done
find . -name '*.po' -print0 | while read -d '' -r file; do msgattrib --output-file="$file" --no-obsolete "$file"; done
