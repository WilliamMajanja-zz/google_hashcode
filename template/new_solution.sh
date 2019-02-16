#!/bin/bash -e
if [ "$#" -ne 1 ] && [ "$#" -ne 2 ]; then
    echo "Usage: ./new_problem.sh <test_name> [solution_name]"
    exit
fi

solution_name=$1

if [ "$#" == 2 ]; then
  solution_name=$2
fi

mkdir output/$1
echo "output" > output/$1/output.txt

mkdir solutions/$solution_name
cp templates/* solutions/$solution_name/
if [[ "$OSTYPE" == "darwin"* ]]; then
  sed -i '' -e "s/<<test_name>>/$1/g" solutions/$solution_name/*
else
  sed -i -e "s/<<test_name>>/$1/g" solutions/$solution_name/*
fi
mkdir solutions/$solution_name/logs
echo "debug log" > solutions/$solution_name/logs/debug.log
