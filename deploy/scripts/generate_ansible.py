import json
import sys

# usage:
# terraform output -json | python generate_ansible.py

ansible_template = """[logbrokers]
{}
[logbrokers:vars]
{jump_host}
[clickhousers]
{}
[clickhousers:vars]
{jump_host}
# balancer - {balancer}"""

host_template = "{type}-{index} ansible_host={ip} ansible_user=btkz ansible_ssh_private_key_file=~/.ssh/id_rsa"


def make_strings(type, ips):
    return "\n".join([host_template.format(type=type, index=index, ip=ip) 
        for index, ip in enumerate(ips)])


def main():
    json_str = ""
    for line in sys.stdin:
        json_str += line
    data = json.loads(json_str)

    nat_instance_ip = data["external_ip_address_nat-instance"]["value"]
    clickhouse_ip = data["internal_ip_address_clickhouse"]["value"]
    balancer_ip = data["external_ip_balancer"]["value"][0]["address"]

    jump_host = """ansible_ssh_common_args='-o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null -o ProxyCommand="ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null -i ~/.ssh/id_rsa -W %h:%p -q btkz@{}"'""".format(nat_instance_ip)

    logbrokers_ips_strs = make_strings("logbroker", data["internal_ip_addresses_logbroker"]["value"])
    clickhouse_ip_str = host_template.format(type="clickhouse", index=0, ip=clickhouse_ip)
    all_params = [logbrokers_ips_strs, clickhouse_ip_str]

    with open('ansible_inventory', 'w') as f:
        print(ansible_template.format(*all_params, jump_host=jump_host, balancer=balancer_ip), file=f)

if __name__ == "__main__":
    main()
