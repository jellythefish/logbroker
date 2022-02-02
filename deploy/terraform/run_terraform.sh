#!/bin/bash

terraform apply -var-file="~/.tfvars" -auto-approve
terraform output -json | python3 ../scripts/generate_ansible.py
mv ./ansible_inventory ../scripts
