from flask import Flask, render_template
from flask_socketio import SocketIO

app = Flask(__name__)
app.config['SECRET_KEY'] = '\xc9\xab\xd3\xe4\xe21\x00\x84\x94*t\xcd'
socketio = SocketIO(app)

@app.route('/')
def sessions():
    return render_template('session.html')

def messageReceived(methods=['GET', 'POST']):
    print('Message was received.')
    
@socketio.on('my event')
def handle_my_custom_event(json, methods=['GET', 'POST']):
    print('Received my event : ' + str(json))
    socketio.emit('my response', json, callback=messageReceived)
    
if __name__ == '__main__':
    socketio.run(app, debug=True)
