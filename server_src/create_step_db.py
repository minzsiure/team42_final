import requests
import sqlite3
from datetime import datetime

step_db = '/var/jail/home/team42/608_team42_final/step_data.db'

def create_route_data():
    conn = sqlite3.connect(step_db)
    c = conn.cursor()
    c.execute(
        """CREATE TABLE IF NOT EXISTS step_table (username text, step integer, date timestamp);""")
    conn.commit()  # commit commands
    conn.close()  # close connection to database


def request_handler(request):
    if request["method"] == "GET":
        with sqlite3.connect(step_db) as c:
            return c.execute('''SELECT * FROM step_table;''',).fetchall()
    if request["method"] == "POST":
        create_route_data()
        username = request['form']['username']
        step = int(request['form']['step'])
        date_now = str(datetime.now().date())
        with sqlite3.connect(step_db) as c:
            matched_users = c.execute(
                '''SELECT * FROM step_table WHERE username = ?;''', (username,)).fetchall()
            #add new row
            if len(matched_users) == 0 or date_now not in [i[2] for i in matched_users]:
                c.execute(
                    '''INSERT into step_table VALUES (?,?,?);''', (username, step, datetime.now().date()))
            #update existing row
            else: 
                udpate_query = ''' UPDATE step_table SET step = step + ? WHERE username == ? AND date == ?'''
                # return username, step, date_now, matched_users[0][0], udpate_query
                c.execute(udpate_query, (step, matched_users[0][0], str(datetime.now().date())))
                return c.execute('''SELECT * FROM step_table;''',).fetchall()
