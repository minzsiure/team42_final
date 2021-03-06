import sqlite3
import urllib.request
from PIL import Image
import base64
import json

visits_db = '/var/jail/home/team42/608_team42_final/user_image.db'


def create_database():
    # connect to that database (will create if it doesn't already exist)
    conn = sqlite3.connect(visits_db)
    c = conn.cursor()  # move cursor into database (allows us to execute commands)
    outs = ""
    c.execute('''CREATE TABLE IF NOT EXISTS image_table (location_name text, user_id int, image_encoding text);''')  # run a CREATE TABLE command
    conn.commit()  # commit commands
    conn.close()  # close connection to database
    return outs


create_database()


def insert_image(location, user_id, image_encoding):
    # connect to that database (will create if it doesn't already exist)
    conn = sqlite3.connect(visits_db)
    c = conn.cursor()  # move cursor into database (allows us to execute commands)
    outs = ""
    c.execute('''INSERT into image_table VALUES (?,?,?);''',
              (location, user_id, image_encoding))
    conn.commit()  # commit commands
    conn.close()  # close connection to database
    return outs


def request_handler(request):
    if request['method'] == 'POST':
        try:
            data = eval(request['data'])
            location = data['location']
            user_id = data['user_id']
            image_encoding = data['image_encoding']
            insert_image(location, user_id, image_encoding)
        except:
            return "Missing value: location, user_id, and/or image_encoding"
        return json.dumps(request['data'])
        # return {'location': location, 'user_id': user_id, 'image_encoding': image_encoding}
    elif request['method'] == 'GET':
        location = request['values']['location'] if 'location' in request['values'] else None
        user_id = request['values']['user_id'] if 'user_id' in request['values'] else None
        img = request['values']['image'] if 'image' in request['values'] else None
        # connect to that database (will create if it doesn't already exist)
        conn = sqlite3.connect(visits_db)
        c = conn.cursor()  # move cursor into database (allows us to execute commands)
        if location and user_id and location != 'all' and user_id != 'all':
            all_image = c.execute(
                '''SELECT * FROM image_table WHERE location_name = ? AND user_id = ?''', (location, user_id)).fetchall()
        elif location and location != 'all':
            all_image = c.execute(
                '''SELECT * FROM image_table WHERE location_name = ? ''', (location, )).fetchall()
        elif user_id and user_id != 'all':
            all_image = c.execute(
                '''SELECT * FROM image_table WHERE user_id = ? ''', (user_id, )).fetchall()
        else:
            all_image = c.execute('''SELECT * FROM image_table''').fetchall()
        out = {}
        if location == 'all':
            out['location'] = [image[0] for image in all_image]
            return json.dumps(out)
        elif location:
            out['location'] = location
        if user_id == 'all':
            out['user'] = [image[1] for image in all_image]
            return json.dumps(out)
        elif user_id:
            out['user'] = user_id
        out['image'] = []
        for image in all_image:
            out['image'].append(
                "<div class=\"d-flex align-items-baseline\"><img height = \"300\" class=\"rounded\" src='data:image/png;base64, " + image[2] + "'/></div>")
        conn.commit()  # commit commands
        conn.close()  # close connection to database
        if img:
            return out['image']
        return json.dumps(out)
