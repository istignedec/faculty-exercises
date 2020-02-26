#!flask/bin/python
import sys
from flask import Flask, jsonify, abort, request, make_response, url_for, current_app
from flask_httpauth import HTTPBasicAuth

app = Flask(__name__)
auth = HTTPBasicAuth()

@auth.get_password
def get_password(username):
    if username == 'ivan':
        return 'stignedec'
    return None

@auth.error_handler
def unauthorized():
    return make_response(jsonify({'error': 'Unauthorized access'}), 401)

artists = [
    {
        'id': 1,
        'name': u'Rihanna',
        'officialweb': u'http://rihannanow.com/'
    },
    {
        'id': 2,
        'name': u'Petar Graso',
        'officialweb': u'https://www.petargraso.com/'
    },
    {
        'id': 3,
        'name': u'Mick Jagger',
        'officialweb': u'http://www.mickjagger.com/'
    },
    {
        'id': 4,
        'name': u'Johnny Cash',
        'officialweb': u'https://www.johnnycash.com/'
    }
]

genres = [
    {
        'id': 1,
        'name': u'pop',
        'artists': [
            {
                'id': 1,
            },
            {
                'id': 2,
            }
        ]
    },
    {
        'id': 2,
        'name': u'rock',
        'artists': [
            {
                'id': 3,
            }
        ]
    },
    {
        'id': 3,
        'name': u'country',
        'artists': [
            {
                'id': 4,
            }
        ]
    }
]


@app.errorhandler(400)
def not_found(error):
    return make_response(jsonify({'error': 'Bad request'}), 400)

@app.errorhandler(404)
def not_found(error):
    return make_response(jsonify({'error': 'Not found'}), 404)


def make_public_artist(artist):
    new_artist = {}
    for field in artist:
        if field == 'id':
            new_artist['uri'] = url_for('get_artist', artist_id=artist['id'], _external=True)
        else:
            new_artist[field] = artist[field]
    return new_artist

@app.route('/music/api/artists', methods=['GET'])
def get_artists():
    return jsonify({'artists': [make_public_artist(artist) for artist in artists]})

def make_public_genre(genre):
    new_genre = {}
    for field in genre:
        if field == 'id':
            new_genre['uri'] = url_for('get_genre', genre_id=genre['id'], _external=True)
        elif field == 'artists':
            # za sve artiste iz zanra dohvati
            list = []
            for genre_artist in genre['artists']:
                for artist in artists:
                    if int(genre_artist['id']) == int(artist['id']):
                        list.append(artist)
                    
            new_genre['artists'] = [make_public_artist(artist) for artist in list]
        else:
            new_genre[field] = genre[field]                
    return new_genre

@app.route('/music/api/genres', methods=['GET'])
def get_genres():
    return jsonify({'genres': [make_public_genre(genre) for genre in genres]})


@app.route('/music/api/artists/<int:artist_id>', methods=['GET'])
def get_artist(artist_id):
    artist = [artist for artist in artists if artist['id'] == artist_id]
    if len(artist) == 0:
        abort(404)
    return jsonify({'artist': make_public_artist(artist[0])})

@app.route('/music/api/genres/<int:genre_id>', methods=['GET'])
def get_genre(genre_id):
    genre = [genre for genre in genres if genre['id'] == genre_id]
    if len(genre) == 0:
        abort(404)
    return jsonify({'genre': make_public_genre(genre[0])})

@app.route('/music/api/genres/<int:genre_id>/artists', methods=['GET'])
def get_artists_for_genre(genre_id):
    genre = [genre for genre in genres if genre['id'] == genre_id]
    if len(genre) == 0:
        abort(404)
        
    list = []
    for artist_genre in genre[0]['artists']:
        for artist in artists:
            if artist_genre['id'] == artist['id']:
                list.append(artist)
    
    if len(list) == 0:
        abort(404)
    return jsonify({'artists': [make_public_artist(artist) for artist in list]})


@app.route('/music/api/artists', methods=['POST'])
@auth.login_required
def create_artist():
    if not request.json or not 'name' in request.json:
        abort(400)
    artist = {
        'id': artists[-1]['id'] + 1,
        'name': request.json['name'],
        'officialweb': request.json.get('officialweb', "")
    }
    artists.append(artist)
    return jsonify({'artist': make_public_artist(artist)}), 201

@app.route('/music/api/genres', methods=['POST'])
@auth.login_required
def create_genre():
    if not request.json or not 'name' in request.json:
        abort(400)
    genre = {
        'id': genres[-1]['id'] + 1,
        'name': request.json['name'],
        'artists': ""
    }
    genres.append(genre)
    return jsonify({'genre': make_public_genre(genre)}), 201


@app.route('/music/api/artists/<int:artist_id>', methods=['PUT'])
@auth.login_required
def update_artist(artist_id):
    artist = [artist for artist in artists if artist['id'] == artist_id]
    if len(artist) == 0:
        abort(404)
    if not request.json:
        abort(400)
    if 'name' in request.json and type(request.json['name']) != str:
        abort(400)
    if 'officialweb' in request.json and type(request.json['officialweb']) is not str:
        abort(400)
    if not 'name' or not 'officialweb' in request.json:
        abort(400)
    artist[0]['name'] = request.json.get('name', artist[0]['name'])
    artist[0]['officialweb'] = request.json.get('officialweb', artist[0]['officialweb'])
    return jsonify({'artist': make_public_artist(artist[0])})

@app.route('/music/api/genres/<int:genre_id>', methods=['PUT'])
@auth.login_required
def update_genre(genre_id):
    genre = [genre for genre in genres if genre['id'] == genre_id]
    if len(genre) == 0:
        abort(404)
    if not request.json:
        abort(400)
    if 'name' in request.json and type(request.json['name']) != str:
        abort(400)
    if not 'name' or not 'artists' in request.json:
        abort(400)
    
    list = []
    if 'artists' in request.json:
        for request_artist in request.json.get('artists'): # za jednog trazenog artista
            for artist in artists: # prodji kroz sve
                if int(artist['id']) == int(request_artist['id']): # nadji artista s tim id-em i stavi ga u listu
                    list.append(artist)

    if len(list) == 0:
        abort(400)
    
    genre[0]['name'] = request.json.get('name', genre[0]['name'])
    if 'artists' in request.json:
        genre[0]['artists'] = list # puni artist iz artists, ne iz dobivenog zanra
    return jsonify({'genre': make_public_genre(genre[0])})

@app.route('/music/api/artists/<int:artist_id>', methods=['DELETE'])
@auth.login_required
def delete_artist(artist_id):
    artist = [artist for artist in artists if artist['id'] == artist_id]
    if len(artist) == 0:
        abort(404)
    artists.remove(artist[0])
    return jsonify({'result': True})

@app.route('/music/api/genres/<int:genre_id>', methods=['DELETE'])
@auth.login_required
def delete_genre(genre_id):
    genre = [genre for genre in genres if genre['id'] == genre_id]
    if len(genre) == 0:
        abort(404)
    genres.remove(genre[0])
    return jsonify({'result': True})

@app.route('/music/api')
def index():
    return current_app.send_static_file('api_doc.html')

if __name__ == '__main__':
    app.run(debug=True)
