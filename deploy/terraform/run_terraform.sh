#!/bin/bash

terraform apply -auto-approve
terraform output -json | python3 ../scripts/generate_ansible.py
mv ./ansible_inventory ../scripts
