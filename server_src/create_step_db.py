import requests
import sqlite3
import json
from datetime import datetime, timedelta, timezone

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
        if 'view' in request['values'].keys():
            view = int(request['values']['view'])
            
            # all user all time
            if view == 1: 
                with sqlite3.connect(step_db) as c:
                    table = c.execute('''SELECT * FROM step_table ORDER BY step DESC;''').fetchall()
                return json.dumps(table)
            
            # all user one day
            if view == 2: 
                date = request['values']['date']
                datetime_object = datetime.strptime(date, '%Y-%m-%d').date()
                with sqlite3.connect(step_db) as c:
                    table = c.execute(
                        '''SELECT * FROM step_table WHERE date == ?;''', (datetime_object, )).fetchall()
                return table
            
            # one user all time
            if view == 3: 
                username = request['values']['username']
                with sqlite3.connect(step_db) as c:
                    table = c.execute(
                        '''SELECT * FROM step_table WHERE username == ?;''', (username, )).fetchall()
                return table
            
            # one user one day
            if view == 4: 
                date = request['values']['date']
                datetime_object = datetime.strptime(date, '%Y-%m-%d').date()
                username = request['values']['username']  # 8986105
                with sqlite3.connect(step_db) as c:
                    table = c.execute(
                        '''SELECT * FROM step_table WHERE username == ? AND date == ?;''', (username, datetime_object)).fetchall()
                return table
                
    if request["method"] == "POST":
        create_route_data()
        username = request['form']['username']
        step = int(request['form']['step'])
        date_now = str(datetime.now(timezone(timedelta(hours=-4), 'EST')).date())
        with sqlite3.connect(step_db) as c:
            matched_users = c.execute(
                '''SELECT * FROM step_table WHERE username = ?;''', (username,)).fetchall()
            #add new row
            if len(matched_users) == 0 or date_now not in [i[2] for i in matched_users]:
                c.execute(
                    '''INSERT into step_table VALUES (?,?,?);''', (username, step, datetime.now(timezone(timedelta(hours=-4), 'EST')).date()))
                return f'{username}: {step}'
            #update existing row
            else: 
                udpate_query = ''' UPDATE step_table SET step = step + ? WHERE username == ? AND date == ?;'''
                # return username, step, date_now, matched_users[0][0], udpate_query
                c.execute(udpate_query, (step, matched_users[0][0], date_now))
                step = c.execute(
                    '''SELECT step FROM step_table WHERE username = ? AND date == ?;''', (matched_users[0][0], date_now)).fetchone()
                return f'{username}: {step[0]}'
