import sqlite3

visits_db = '/var/jail/home/team42/608_team42_final/user_image.db'

def create_database():
    conn = sqlite3.connect(visits_db)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  # move cursor into database (allows us to execute commands)
    outs = ""
    c.execute('''CREATE TABLE IF NOT EXISTS image_table (location_name text, user int, image_url text);''') # run a CREATE TABLE command
    c.execute('''INSERT into image_table VALUES (?,?,?);''', ('Student Center', 0, 'https://studentlife.mit.edu/sites/default/files/styles/tier_2_header/public/IMG_0371_0.JPG?itok=vmpBmWcD'))
    c.execute('''INSERT into image_table VALUES (?,?,?);''', ('Infinite Corridor/Killian', 1, 'https://upload.wikimedia.org/wikipedia/commons/2/27/Infinitecorridor.jpg'))
    conn.commit() # commit commands
    conn.close() # close connection to database
    return outs

def insert_image(location, user_id, image_url):
    conn = sqlite3.connect(visits_db)  # connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  # move cursor into database (allows us to execute commands)
    outs = ""
    c.execute('''INSERT into image_table VALUES (?,?,?);''', (location, user_id, image_url))
    conn.commit() # commit commands
    conn.close() # close connection to database
    return outs

def request_handler(request):
    if request['method'] == 'POST':
        try:
            location = request['form']['location']
            user_id = request['form']['user_id']
            image_url = request['form']['image_url']
            insert_image(location, user_id, image_url)
        except:
            return "Lack one or more arguments: location, user_id, image_url."
        return {'location':location, 'user_id': user_id, 'image_url':image_url}
    elif request['method'] == 'GET':
        location = request['values']['location'] if 'location' in request['values'] else None
        user_id = request['values']['user_id'] if 'user_id' in request['values'] else None
        conn = sqlite3.connect(visits_db)  # connect to that database (will create if it doesn't already exist)
        c = conn.cursor()  # move cursor into database (allows us to execute commands)
        if location and user_id:
            all_image = c.execute('''SELECT * FROM image_table WHERE location_name = ? AND user = ?''', (location, user_id)).fetchall()
        elif location:
            all_image = c.execute('''SELECT * FROM image_table WHERE location_name = ? ''', (location, )).fetchall()
        elif user_id:
            all_image = c.execute('''SELECT * FROM image_table WHERE user = ? ''', (user_id, )).fetchall()
        else:
            return "Lack any of the variables: location, user_id."
        out = {}
        if location:
            out['location'] = location
        if user_id:
            out['user'] = user_id
        out = [image for image in all_image]
        conn.commit() # commit commands
        conn.close() # close connection to database
        return out


