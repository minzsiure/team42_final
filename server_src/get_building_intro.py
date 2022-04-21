import requests
import sqlite3
import json
ht_db = '/var/jail/home/team42/608_team42_final/building_data.db'


def request_handler(request):
    if request["method"] == "POST":
        return None
    elif request['method'] == "GET":
        maxlen = int(request['values']['max_len'])
        building_id = request['values']['building_id']
        with sqlite3.connect(ht_db) as c:
            c.execute(
                """CREATE TABLE IF NOT EXISTS building_table (building_name text, building_id text, coordinates text, text_intro text, audio_data text);""")
            routes = c.execute(
                '''SELECT text_intro FROM building_table WHERE building_id = ?;''', (building_id,)).fetchall()

            result = {}
            text = routes[0][0]

            if maxlen > len(text):
                maxlen = len(text)
            text = text[0:maxlen]
            while(maxlen > 0 and text[maxlen-1] != '.'):
                maxlen -= 1
                text = text[0:maxlen]
            result["text_intro"] = text

            # return JSON object with 1. number of routes, and 2. name of each route
            return json.dumps(result)
