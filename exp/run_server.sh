#!/bin/bash

server_name="test"
env="macos-clang-12.0.5"
base_path=/Users/zhanghaonan/tal_project/EXPGraphCompute/GraphCompute
graph_define_directory=$base_path/graph_define/exp_v5
log_directory=$base_path/log/$server_name
result_directory=$base_path/result/$server_name
config_path=$base_path/config/$server_name


if [ ! -d "$log_directory" ]; then
        mkdir $log_directory
fi

if [ ! -d "$result_directory" ]; then
        mkdir $result_directory
fi

if [ ! -d "$config_path" ]; then
        mkdir $config_path
fi

python config_generate.py -server_name $server_name \
                          -graph_define_directory $graph_define_directory \
                          -log_directory=$log_directory \
                          -result_directory=$result_directory \
                          -config_path=$config_path

../bini/$env/GraphCompute --config_file_path=$base_path/config/${server_name}.ini


