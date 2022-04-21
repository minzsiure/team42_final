import requests
import sqlite3
import json
ht_db = '/var/jail/home/team42/608_team42_final/route_data.db'


def request_handler(request):
    if request["method"] == "POST":
        return None
    elif request['method'] == "GET":
        search_id = int(request['values']['route_id'])

        with sqlite3.connect(ht_db) as c:
            c.execute(
                """CREATE TABLE IF NOT EXISTS route_table (route_name text, route_id integer, route_building_names text, route_building_id text, route_center_coordinates text);""")
            routes = c.execute(
                '''SELECT route_building_names, route_center_coordinates FROM route_table WHERE route_id = ?;''', (search_id,)).fetchall()

            result = {}
            result["building_names"] = eval(routes[0][0])
            result["route_center_coordinates"] = eval(routes[0][1])

            # return JSON object with 1. list of buildings in the selected route, and
            # 2. list of center cooridnates of the buildings in order
            return json.dumps(result)
