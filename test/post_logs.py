import requests
import json


def main():
    table_name = "kek"
    port = 80
    url = "http://localhost:{}/write_log".format(port)
    headers = {
        "Content-Type": "application/x-www-form-urlencoded"
    }

    all_entries = []
    for i in range(0, 9, 4):
        all_entries.extend([
            {
                "table_name": table_name,
                "rows": [
                    {"a": i, "b": "some new row {}".format(i)},
                    {"a": i + 1, "b": "some new row {}".format(i + 1)}
                ],
                "format": "json"
            },
            {
                "table_name": table_name,
                "rows": [
                    [i + 2, "row from list {}".format(i + 2)],
                    [i + 3, "row from list {}".format(i + 3)],
                ],
                "format": "list"
            }
        ])

    for i in range(200):
        req = requests.post(url, headers=headers, data=json.dumps(all_entries))
        print(req.status_code)


if __name__ == '__main__':
    main()
