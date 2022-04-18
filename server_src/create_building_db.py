import requests
import sqlite3
ht_db = '/var/jail/home/team42/608_team42_final/building_data.db'

# hayden
Hayden = {"building_name": "Hayden Library",
          "building_id": "14S-100",
          "coordinates": [(42.359023086477464, -71.08962615615349), (42.35928816534441, -71.08881216918246), (42.35909654217911, -71.0886940330203), (42.35882507502796, -71.0895296302649)],
          "text_intro": "The Hayden Library, located along the Charles River, is home to the Humanities and Science collections. It also boasts some of the best spaces on campus for studying.  Students love spreading out on large tables in front of the floor-to-ceiling windows, with inspiring views of the Charles River and Boston beyond.",
          "audio_data": []
          }
# media lab
Media_Lab = {"building_name": "Media Lab",
             "building_id": "E14",
             "coordinates": [(42.360960871134, -71.08805145867707), (42.3601684428233, -71.08757433423418), (42.36040732559083, -71.0868764839976), (42.361145593892694, -71.08746486502764)],
             "text_intro": "Founded in 1985, the MIT Media Lab is one of the world’s leading research and academic organizations. Unconstrained by traditional disciplines, Media Lab designers, engineers, artists, and scientists strive to create technologies and experiences that enable people to understand and transform their lives, communities, and environments.",
             "audio_data": []
             }
# stata
Stata = {"building_name": "Stata Center",
         "building_id": "32",
         "coordinates": [(42.362072776481696, -71.0898875508796), (42.361344045156535, -71.08989698848988), (42.36140332026907, -71.09031224334224), (42.36114878556667, -71.09060009045581), (42.36124292865895, -71.09116162826751), (42.36144864825861, -71.09138813091427), (42.36169620824046, -71.09156272671758), (42.362152971279414, -71.0904443698993)],
         "text_intro": "The 400,000-square-foot Ray and Maria Stata Center for Computer Science and Artificial Intelligence Laboratory houses labs, conference halls, classrooms, offices, food courts, and childcare and recreation centers, and bears all the hallmark characteristics of its architect – Frank O. Gehry. The exterior of the building features tilting towers, many-angled walls and whimsical shapes, while the interior space appears curved, bulging, segmental, and twisted. ",
         "audio_data": []
         }

building_list = [Hayden, Media_Lab, Stata]


def create_building_data():
    conn = sqlite3.connect(ht_db)
    c = conn.cursor()
    c.execute(
        """CREATE TABLE IF NOT EXISTS building_table (building_name text, building_id text, coordinates text, text_intro text, audio_data text);""")
    # some other query(ies) about inserting data
    for building in building_list:
        c.execute('''INSERT into building_table VALUES (?,?,?,?,?);''',
                  (building["building_name"], building["building_id"], repr(building['coordinates']), repr(building['text_intro']), repr(building['audio_data']),))

    show = c.execute(
        '''SELECT DISTINCT * FROM building_table;''').fetchall()
    conn.commit()  # commit commands
    conn.close()  # close connection to database
    return show


def request_handler(request):
    if request["method"] == "POST":
        create_building_data()
    elif request['method'] == "GET":
        with sqlite3.connect(ht_db) as c:
            c.execute(
                """CREATE TABLE IF NOT EXISTS building_table (building_name text, building_id text, coordinates text, text_intro text, audio_data text);""")
            locations = c.execute(
                '''SELECT DISTINCT * FROM building_table;''').fetchall()
            all_coordinates = [eval(locations[i][2])
                               for i in range(len(locations))]
            all_text_intros = [eval(locations[i][3])
                               for i in range(len(locations))]
            return all_coordinates, all_text_intros
