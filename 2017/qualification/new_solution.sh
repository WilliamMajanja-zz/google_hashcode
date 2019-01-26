#!/bin/bash -e
if [ "$#" -ne 1 ]; then
    echo "Usage: ./new_problem.sh <test_name>"
    exit
fi

mkdir output/$1
echo "output" > output/$1/output.txt

mkdir solutions/$1
cp templates/* solutions/$1/
if [[ "$OSTYPE" == "darwin"* ]]; then
  sed -i '' -e "s/<<test_name>>/$1/g" solutions/$1/*
else
  sed -i -e "s/<<test_name>>/$1/g" solutions/$1/*
fi
mkdir solutions/$1/logs
