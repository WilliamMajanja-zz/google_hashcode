#!/bin/bash -e
if [ "$#" -ne 2 ]; then
    echo "Usage: ./new_problem.sh <test_name> <TestName>"
    exit
fi

mkdir output/$1

mkdir solutions/$1
cp templates/* solutions/$1/
if [[ "$OSTYPE" == "darwin"* ]]; then
  sed -i '' -e "s/<<test_name>>/$1/g" solutions/$1/*
  sed -i '' -e "s/<<TestName>>/$2/g" solutions/$1/*
else
  sed -i -e "s/<<test_name>>/$1/g" solutions/$1/*
  sed -i -e "s/<<TestName>>/$2/g" solutions/$1/*
fi
mkdir solutions/$1/logs
