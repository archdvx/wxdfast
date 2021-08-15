#!/usr/bin/env bash

for i in *.po
do
  echo "Generating ${i%.po}.mo ..."
  rm ${i%.po}.mo
  msgfmt $i -o ${i%.po}.mo
done
