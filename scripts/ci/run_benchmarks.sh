#!/bin/bash
set -eou pipefail

function run_benchmarks_for_folder {
    echo "Running benchmarks for $1"

    FOLDER=$1
    RESULTS_FOLDER=$2

    mkdir -p $RESULTS_FOLDER

    BENCHMARKS_FOLDER="$FOLDER/build/src/benchmarks"

    # ./$BENCHMARKS_FOLDER/match-bench "./$FOLDER/test/data/mld/monaco.osrm" mld > "$RESULTS_FOLDER/match_mld.bench"
    # ./$BENCHMARKS_FOLDER/match-bench "./$FOLDER/test/data/ch/monaco.osrm" ch > "$RESULTS_FOLDER/match_ch.bench"
    # ./$BENCHMARKS_FOLDER/route-bench "./$FOLDER/test/data/mld/monaco.osrm" mld > "$RESULTS_FOLDER/route_mld.bench"
    # ./$BENCHMARKS_FOLDER/route-bench "./$FOLDER/test/data/ch/monaco.osrm" ch > "$RESULTS_FOLDER/route_ch.bench"
    # ./$BENCHMARKS_FOLDER/alias-bench > "$RESULTS_FOLDER/alias.bench"
    # ./$BENCHMARKS_FOLDER/json-render-bench  "./$FOLDER/src/benchmarks/portugal_to_korea.json" > "$RESULTS_FOLDER/json-render.bench"
    # ./$BENCHMARKS_FOLDER/packedvector-bench > "$RESULTS_FOLDER/packedvector.bench"
    # ./$BENCHMARKS_FOLDER/rtree-bench "./$FOLDER/test/data/monaco.osrm.ramIndex" "./$FOLDER/test/data/monaco.osrm.fileIndex" "./$FOLDER/test/data/monaco.osrm.nbg_nodes" > "$RESULTS_FOLDER/rtree.bench"

    # TODO: CH
    BINARIES_FOLDER="$FOLDER/build"
    echo "PWD: $FOLDER"
    cp ~/data.osm.pbf $FOLDER
    $BINARIES_FOLDER/osrm-extract -p $FOLDER/profiles/car.lua $FOLDER/data.osm.pbf
    $BINARIES_FOLDER/osrm-partition $FOLDER/data.osrm
    $BINARIES_FOLDER/osrm-customize $FOLDER/data.osrm
    $BINARIES_FOLDER/osrm-contract $FOLDER/data.osrm

    # TODO: save results
    if [ -f "$FOLDER/scripts/ci/locustfile.py" ]; then
        $BINARIES_FOLDER/osrm-routed --algorithm mld $FOLDER/data.osrm &
        OSRM_ROUTED_PID=$!

        curl --retry-delay 3 --retry 10 --retry-all-errors "http://127.0.0.1:5000/route/v1/driving/13.388860,52.517037;13.385983,52.496891?steps=true"
        locust -f $FOLDER/scripts/ci/locustfile.py \
            --headless \
            --processes -1 \
            --users 10 \
            --spawn-rate 1 \
            --host http://localhost:5000 \
            --run-time 1m \
            --csv=results \
            --loglevel ERROR

        echo "STATS: "
        cat results_stats.csv
        # echo "FAILURES: "
        # cat results_failures.csv
        # echo "EXCEPTIONS: "
        # cat results_exceptions.csv
        # echo "STATS HISTORY: "
        # cat results_stats_history.csv
# 2024-05-26T16:48:29.2826130Z Type,Name,Request Count,Failure Count,Median Response Time,Average Response Time,Min Response Time,Max Response Time,Average Content Size,Requests/s,Failures/s,50%,66%,75%,80%,90%,95%,98%,99%,99.9%,99.99%,100%
# 2024-05-26T16:48:29.2827761Z GET,/route/v1/driving,7668,0,4,15.894057099894761,1.168323000001692,137.2044539999706,26387.300208659362,127.83539601258259,0.0,4,6,44,45,47,48,49,50,100,140,140
# 2024-05-26T16:48:29.2829001Z ,Aggregated,7668,0,4,15.894057099894761,1.168323000001692,137.2044539999706,26387.300208659362,127.83539601258259,0.0,4,6,44,45,47,48,49,50,100,140,140

        kill -0 $OSRM_ROUTED_PID
    fi
}

# run_benchmarks_for_folder $1 "${1}_results"
run_benchmarks_for_folder $2 "${2}_results"

