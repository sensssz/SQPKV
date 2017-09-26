#!/bin/bash

num_requests=30000
think_time=100

sleep_for() {
  printf "Sleeping fo ${1}s"
  for((i = 0; i < ${1}; i++))
  do
    printf '.'
    sleep 1s
  done
  echo ''
}

kill_oustanding_job() {
  jname=`bjobs | awk 'NR==2{print \$7}'`
  if [ -z "jname" ] || [ "$jname" != "sqpkv" ]; then
    return
  fi
  jid=`bjobs | awk 'NR==2{print \$1}'`
  if [ ! -z "${jid}" ]; then
    bkill $@ ${jid}
  fi
  sleep_for 10
}

run_exp() {
  suffix=${1}
  kill_oustanding_job
  bsub < ../run_${suffix}.lsf
  sleep_for 10

  mkdir -p latencies/${suffix}
  mkdir -p latencies/kv_latencies
  mkdir -p latencies/send_latencies

  ./exponential_client --server_addr_file=${HOME}/kvs/proxy --lambda=0.05 --think_time=${think_time} --latency_file=latencies/${suffix}/latency0

  for ((i = 0; i < 5; i++))
  do
    echo '' > sqpkv.log
    ./exponential_client --server_addr_file=${HOME}/kvs/proxy --lambda=0.05 --think_time=${think_time} --latency_file=latencies/${suffix}/latency${i}
    cp sqpkv.log latencies/send_latencies/latency${i}
  done

  rm -rf latencies/send_latencies_${suffix}
  mv latencies/send_latencies latencies/send_latencies_${suffix}
  rm -rf latencies/kv_latencies_${suffix}
  mv latencies/kv_latencies latencies/kv_latencies_${suffix}

  kill_oustanding_job
}

cd build

for suffix in sqp normal
do
  run_exp ${suffix}
done
