#!/bin/bash

./run_ansible.sh install_docker.yml && \
./run_ansible.sh deploy_clickhouse.yml && \
./run_ansible.sh deploy_logbroker.yml
