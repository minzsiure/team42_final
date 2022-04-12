import requests
import sqlite3
ht_db = '/var/jail/home/team42/608_team42_final/fake_data.db'
# ht_db = '/Users/yixie/Desktop/6.08_Team42_FinalProject/fake_data.db'


def create_fake_data():
    location_1 = {"location_name": "Student Center", "coordinates": [(-71.095863, 42.357307), (-71.097730, 42.359075),
                                                                     (-71.095102, 42.360295), (-71.093900, 42.359340), (-71.093289, 42.358306)], 'text_intro': ['Centrally located, the Stratton Student Center offers places to gather, study, dine, do errands, and relax. MIT students view the Student Center as their out-of-the-classroom home.'], "image_url": [
        'https://studentlife.mit.edu/sites/default/files/styles/tier_2_header/public/IMG_0371_0.JPG?itok=vmpBmWcD'], "audio_data": [1, 2, 3, 4, 5, 6]}

    location_2 = {"location_name": "Infinite Corridor/Killian", "coordinates": [(-71.093932, 42.359542), (-71.092259, 42.357180), (-71.089619, 42.358274), (-71.090928, 42.360541)], 'text_intro': ['The Infinite Corridor is a 251-meter (823 ft) hallway that runs through the main buildings of the Massachusetts Institute of Technology, specifically parts of the buildings numbered 7, 3, 10, 4, and 8 (from west to east).'], "image_url": [
        'https://upload.wikimedia.org/wikipedia/commons/2/27/Infinitecorridor.jpg'], "audio_data": [3, 4, 5, 6, 7]}

    conn = sqlite3.connect(ht_db)
    c = conn.cursor()
    c.execute(
        """CREATE TABLE IF NOT EXISTS fake_table (lcation_name text, coordinates text, text_intro text, image_url text, audio_data text);""")
    # some other query(ies) about inserting data
    c.execute('''INSERT into fake_table VALUES (?,?,?,?,?);''',
              (location_1["location_name"], repr(location_1['coordinates']), repr(location_1['text_intro']), repr(location_1['image_url']), repr(location_1['audio_data'])))
    c.execute('''INSERT into fake_table VALUES (?,?,?,?,?);''',
              (location_2["location_name"], repr(location_2['coordinates']), repr(location_2['text_intro']), repr(location_2['image_url']), repr(location_2['audio_data'])))
    show = c.execute(
        '''SELECT DISTINCT * FROM fake_table;''').fetchall()
    conn.commit()  # commit commands
    conn.close()  # close connection to database
    return show


def request_handler(request):
    if request["method"] == "POST":
        create_fake_data()
    elif request['method'] == "GET":
        with sqlite3.connect(ht_db) as c:
            c.execute(
                """CREATE TABLE IF NOT EXISTS fake_table (lcation_name text, coordinates text, text_intro text, image_url text, audio_data text);""")
            locations = c.execute(
                '''SELECT DISTINCT * FROM fake_table;''').fetchall()
            all_coordinates = [eval(locations[i][1])
                               for i in range(len(locations))]
            all_text_intros = [eval(locations[i][2])
                               for i in range(len(locations))]
            return all_coordinates, all_text_intros


# create_fake_data()
# request = {'method': 'GET', 'args': [], 'values': {}}
# print(request_handler(request))
