import codecs
import requests
import json
import pprint  # prettyprint in python
from bs4 import BeautifulSoup as bs

import os

import re


def get_all_images():
    '''
    this returns all images in the database
    '''
    url = "https://608dev-2.net/sandbox/sc/team42/608_team42_final/image_request.py?image=True"
    r = requests.get(url)
    response = r.json()
    pprint.pprint(response)
    return response


def get_all_users():
    url = "https://608dev-2.net/sandbox/sc/team42/608_team42_final/image_request.py?user_id=all"
    r = requests.get(url)
    response = r.json()
    return list(set(response['user']))


def get_img_for_user(user_name):
    '''
    given a user name, get their corresponding pictures
    '''
    url = "https://608dev-2.net/sandbox/sc/team42/608_team42_final/image_request.py?user_id=" + \
        str(user_name)
    r = requests.get(url)
    response = r.json()
    return list(set(response['image']))


def get_img_for_location(location_name):
    '''
    given a user name, get their corresponding pictures
    '''
    url = "https://608dev-2.net/sandbox/sc/team42/608_team42_final/image_request.py?location=" + \
        str(location_name)
    r = requests.get(url)
    response = r.json()
    return list(set(response['image']))


def get_all_locations():
    '''
    this returns all locations in the database
    '''
    url = "https://608dev-2.net/sandbox/sc/team42/608_team42_final/image_request.py?location=all"
    r = requests.get(url)
    response = r.json()
    return list(set(response['location']))


def create_user_section():
    str_header = "<section id=\"users\"><div class=\"container-fluid\"><div class=\"row\"><div class=\"col-lg-6\"><img class=\"users-img\"src=\"mit_people.jpeg\" alt=\"team_pic\"></div><div class=\"col-lg-6 users-text\"><h3>Avaliable Users</h3>"
    str_2 = "<p> <div class=\"accordion\" id=\"accordionExample\">"
    # fill in cards
    str_end = "</div></p></div></div></div></section>"

    # call users
    all_users = get_all_users()
    card_str = ''
    for user in all_users:
        # for each user, call pictures and call create_card
        all_imgs = get_img_for_user(user)
        card_str += create_card(user, all_imgs)
    # append into the code
    return str_header + str_2 + card_str + str_end


def create_card(user_name, user_pics):
    str_1 = "<div class=\"card\"><div class=\"card-header\" id=\"headingOne\"><h2 class=\"mb-0\"><button class=\"btn btn-link\" type=\"button\" data-toggle=\"collapse\" data-target=\"#collapseOne\" aria-expanded=\"true\" aria-controls=\"collapseOne\">"
    # Collapsible Group Item  # 1
    str_2 = "</button></h2></div><div id=\"collapseOne\" class=\"collapse show\" aria-labelledby=\"headingOne\" data-parent=\"#accordionExample\"><div class=\"card-body\">"
    # PICTURES GO HERE
    str_3 = "</div></div></div>"
    unif_str = ''
    unif_str += str_1 + str(user_name) + str_2 + \
        "<div class=\"container\">"
    for pic in user_pics:
        unif_str += pic
    unif_str += "</div>"
    unif_str += str_3
    return unif_str


def create_location_section():
    str_header = "<section id=\"locations\"><div class=\"container-fluid\"><div class=\"row\"><div class=\"col-lg-6\"><img class=\"locations-img\"src=\"mit_map.png\" alt=\"hmmm\"></div><div class=\"col-lg-6 locations-text\"><h3>Avaliable Locations</h3>"
    str_2 = "<p> <div class=\"accordion\" id=\"accordionExample\">"
    # fill in cards
    str_end = "</div></p></div></div></div></section>"

    # call users
    all_locations = get_all_locations()
    card_str = ''
    for loc in all_locations:
        # for each user, call pictures and call create_card
        all_imgs = get_img_for_location(loc)
        card_str += create_card(loc, all_imgs)
    # append into the code
    return str_header + str_2 + card_str + str_end


def get_all_stepcounts():
    url = "https://608dev-2.net/sandbox/sc/team42/608_team42_final/create_step_db.py?view=1"
    r = requests.get(url)
    response = r.json()
    return response


print(get_all_stepcounts())


def ulify(elements):
    string = ""
    # string += "user\tscore\tcorrect number\twrong number\n"
    string += "<ol>\n"
    for x in elements:
        string += "<li>" + "User: "+str(x[0]) + ", Score:"+str(x[1])+", Date:" + \
            str(x[2]) + "</li>\n"
    string += "</ol>"
    return string


def create_leaderboard():
    str_header = "<section id=\"steps\"><div class=\"container-fluid\"><div class=\"row\"><div class=\"col-lg-6 steps-text\"><h3>Historical Stepcount Leaderboard</h3>"

    # fill in cards
    str_end = "</div></div></div></section>"

    # call all step counts
    elements = get_all_stepcounts()
    content = ulify(elements)
    # create leaderboard

    # append into the code
    return str_header + content + str_end


def request_handler(request):
    '''
    this reads html code
    '''
    file1 = codecs.open(
        '/var/jail/home/team42/608_team42_final/index.html', 'r', "utf-8")
    file2 = codecs.open(
        '/var/jail/home/team42/608_team42_final/index2.html', 'r', "utf-8")
    s1 = file1.read()
    s2 = file2.read()
    users = create_user_section()
    locations = create_location_section()
    steps = create_leaderboard()
    return s1+users+locations+steps+s2


# print(create_user_section())
