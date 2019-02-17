#!/bin/bash -e
if [ "$#" -ne 2 ]; then
    echo "Usage: ./new_problem.sh solution_name default_test"
    exit
fi

solution_name=$1
test_name=$2


mkdir solutions/$solution_name
cp templates/* solutions/$solution_name/
if [[ "$OSTYPE" == "darwin"* ]]; then
  sed -i '' -e "s/<<test_name>>/$test_name/g" solutions/$solution_name/*
else
  sed -i -e "s/<<test_name>>/$test_name/g" solutions/$solution_name/*
fi
mkdir solutions/$solution_name/logs
echo "debug log" > solutions/$solution_name/logs/debug.log

find input -name "*.in" -exec basename {} .in ';' | xargs -I {} bash -c "
mkdir output/{};
echo "output" > output/{}/output.txt;
"
