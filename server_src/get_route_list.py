import requests
import sqlite3
ht_db = '/var/jail/home/team42/608_team42_final/route_data.db'


def request_handler(request):
    if request["method"] == "POST":
        return None
    elif request['method'] == "GET":
        with sqlite3.connect(ht_db) as c:
            c.execute(
                """CREATE TABLE IF NOT EXISTS route_table (route_name text, route_id integer, route_building_names text, route_building_id text);""")
            routes = c.execute(
                '''SELECT DISTINCT * FROM route_table;''').fetchall()

            result = {}
            result["num_routes"] = len(routes)
            result["name_routes"] = [routes[i][0] for i in range(len(routes))]

            # return JSON object with 1. number of routes, and 2. name of each route
            return result
