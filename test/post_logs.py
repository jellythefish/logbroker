import requests
import json

import argparse

# usage: python3 post_logs.py 51.250.11.63:80 100 kek

def main():
    parser = argparse.ArgumentParser(description='send logs to balancer')
    parser.add_argument('host', type=str)
    parser.add_argument('number_of_entries', type=str)
    parser.add_argument('table_name', type=str)
    args = parser.parse_args()

    url = "http://{}/write_log".format(args.host)

    for i in range(0, int(args.number_of_entries), 4):
        entry = [
            {
                "table_name": args.table_name,
                "rows": [
                    {"a": i, "b": "some new row {}".format(i)},
                    {"a": i + 1, "b": "some new row {}".format(i + 1)}
                ],
                "format": "json"
            },
            {
                "table_name": args.table_name,
                "rows": [
                    [i + 2, "row from list {}".format(i + 2)],
                    [i + 3, "row from list {}".format(i + 3)],
                ],
                "format": "list"
            }
        ]
        req = requests.post(url, data=json.dumps(entry))
        print(req.status_code)


if __name__ == '__main__':
    main()
