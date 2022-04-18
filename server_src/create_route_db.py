import requests
import sqlite3
ht_db = '/var/jail/home/team42/608_team42_final/route_data.db'

hayden_tour = {"route_name": "Hayden Tour", "route_id": 1, "route_building_names": [
    "Hayden", "Media Lab", "Stata Center"], "route_building_id": ["14S-100", "E14", "32"], "route_center_coordinates": [(42.35909, -71.08917), (42.36047, -71.08735), (42.36183, -71.09033)]}

route_list = [hayden_tour]


def create_route_data():
    conn = sqlite3.connect(ht_db)
    c = conn.cursor()
    c.execute(
        """CREATE TABLE IF NOT EXISTS route_table (route_name text, route_id integer, route_building_names text, route_building_id text, route_center_coordinates text);""")
    # some other query(ies) about inserting data
    for route in route_list:
        c.execute('''INSERT into route_table VALUES (?,?,?,?,?);''',
                  (route["route_name"], route["route_id"], repr(route['route_building_names']), repr(route['route_building_id']), repr(route["route_center_coordinates"])))

    show = c.execute(
        '''SELECT DISTINCT * FROM route_table;''').fetchall()
    conn.commit()  # commit commands
    conn.close()  # close connection to database
    return show


def request_handler(request):
    if request["method"] == "POST":
        create_route_data()
    elif request['method'] == "GET":
        with sqlite3.connect(ht_db) as c:
            c.execute(
                """CREATE TABLE IF NOT EXISTS route_table (route_name text, route_id integer, route_building_names text, route_building_id text, route_center_coordinates text);""")
            routes = c.execute(
                '''SELECT DISTINCT * FROM route_table;''').fetchall()
            all_names = [routes[i][0]
                         for i in range(len(routes))]
            all_building_names = [eval(routes[i][2])
                                  for i in range(len(routes))]
            return routes
