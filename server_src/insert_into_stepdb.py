import requests
import sqlite3
from datetime import datetime, timedelta

step_db = '/var/jail/home/team42/608_team42_final/step_data.db'

username = 'Kuan Qiao'
date = (datetime.today() - timedelta(days=2)).date()
step = 70


def request_handler(request):
    with sqlite3.connect(step_db) as c:
        c.execute('''INSERT into step_table VALUES (?,?,?);''', (username, step, date))
