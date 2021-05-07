from flask import Flask
from flask import request

gUser = "none"
app = Flask(__name__)

@app.route('/',  methods = ['GET', 'POST'])
def index():
  global gUser
  if request.method == 'GET':
    return 'Hello, Mr.' + gUser
  if request.method == 'POST':
    print(request.get_json());
    if(request.is_json):
      gUser = request.get_json()["username"] 
    return "SUCCESS" 

  
# @app.route('/greet')
# def say_hello():
#   return 'Hello from Server'
