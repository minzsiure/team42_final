import sqlite3
import urllib.request
from PIL import Image
import base64

visits_db = '/var/jail/home/team42/608_team42_final/user_image.db'


def create_database():
    # connect to that database (will create if it doesn't already exist)
    conn = sqlite3.connect(visits_db)
    c = conn.cursor()  # move cursor into database (allows us to execute commands)
    outs = ""
    c.execute('''CREATE TABLE IF NOT EXISTS image_table (location_name text, user int, image_encoding text);''')  # run a CREATE TABLE command
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
            location = request['form']['location']
            user_id = request['form']['user_id']
            image_encoding = request['form']['image_encoding']
            insert_image(location, user_id, image_encoding)
        except:
            return "Lack one or more arguments: location, user_id, image_url."
        return {'location': location, 'user_id': user_id, 'image_encoding': image_encoding}
    elif request['method'] == 'GET':
        location = request['values']['location'] if 'location' in request['values'] else None
        user_id = request['values']['user_id'] if 'user_id' in request['values'] else None
        img = request['values']['image'] if 'image' in request['values'] else None
        # connect to that database (will create if it doesn't already exist)
        conn = sqlite3.connect(visits_db)
        c = conn.cursor()  # move cursor into database (allows us to execute commands)
        if location and user_id:
            all_image = c.execute(
                '''SELECT * FROM image_table WHERE location_name = ? AND user = ?''', (location, user_id)).fetchall()
        elif location:
            all_image = c.execute(
                '''SELECT * FROM image_table WHERE location_name = ? ''', (location, )).fetchall()
        elif user_id:
            all_image = c.execute(
                '''SELECT * FROM image_table WHERE user = ? ''', (user_id, )).fetchall()
        else:
            all_image = c.execute('''SELECT * FROM image_table''').fetchall()
        out = {}
        if location:
            out['location'] = location
        if user_id:
            out['user'] = user_id
        out['image'] = []
        for image in all_image:
            out['image'].append(
                "<div><img src='data:image/png;base64, "+image[2]+"'/></div>")
        conn.commit()  # commit commands
        conn.close()  # close connection to database
        if img:
            return out['image']
        return out
