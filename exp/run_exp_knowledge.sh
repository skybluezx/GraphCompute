server_name="v5_exp_knowledge_new"
prune_name="v5_new_cut_knowledge"
env="centos7-gcc-10.3.0"
use_restartRatios=0
base_path=/home/work/zhanghaonan/graphcompute
exp_input_path=$base_path/exp/prune_input/$prune_name
task_path=$base_path/task/${server_name}
prune_path=$base_path/exp/prune_input/$prune_name
train_json_path=$base_path/task/${server_name}/exp_train_base.json
kp_question_recall_result_directory=$base_path/result/${server_name}/0
exp_result_path=$base_path/exp/result/${server_name}

if [ ! -d "$task_path" ]; then
        mkdir $task_path
fi
if [ ! -d "$exp_result_path" ]; then
        mkdir $exp_result_path
fi

#restartRatios=(0 0.02 0.01 0.005 0.001)
#restartRatios=(0 0.001 0.002 0.005 0.01 0.02 0.03 0.04 0.05)
if [[ $use_restartRatios -eq 1 ]]; then
  restartRatios=(0 0.002 0.004 0.006 0.008 0.01 0.012 0.014 0.016 0.018 0.02 0.022 0.024 0.026 0.028 0.030 0.032 0.034 0.036 0.038 0.04)
  walkLengthRatios=(-10000)
else
  restartRatios=(0)
  walkLengthRatios=(1)
fi

echo running
for file in `ls $exp_input_path`
do
  echo "------------------------------------------------"
  echo "running exp: " $file
  # 重置图
  ../build/bin/$env/GraphComputeClient --server_name=${server_name} --action="RESET"
  echo "reset graph success"
  echo "{\"name\":\"exclude_node_or_edge_from_file\", \"file_path\":\"$prune_path/"$file\""}" > $task_path/exp_graph_prune.json
  ../build/bin/$env/GraphComputeClient --server_name=${server_name} --json=$task_path/exp_graph_prune.json
  echo "prune success"
  # 训练超参
  for restartRatio in ${restartRatios[*]}
  do
    for walkLengthRatio in ${walkLengthRatios[*]}
    do
      python $base_path/py_script/build_train_json.py -file_path ${train_json_path} -restartRatio ${restartRatio} -walkLengthRatio ${walkLengthRatio}
      ../build/bin/$env/GraphComputeClient --server_name=${server_name} --json=${train_json_path}
      if [[ $use_restartRatios -eq 1 ]]; then
        python $base_path/py_script/result_check_base.py -file_path ${kp_question_recall_result_directory} > $base_path/exp/result/${server_name}/GC_${restartRatio}_${walkLengthRatio}_$file
      else
        python $base_path/py_script/result_check_base.py -file_path ${kp_question_recall_result_directory} > $base_path/exp/result/${server_name}/GC_$file
      fi
    done
  done
done
