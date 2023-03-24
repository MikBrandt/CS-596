"""
	This code exists on the AWS EC2 instance, it is not used on my local machine.
"""

from flask import Flask
from flask import request

app = Flask(__name__)

@app.route("/")
def hello():
        print("temperature:", request.args.get("temperature"), "\thumidity:", request.args.get("humidity"))
        return "Temperature: "+str(request.args.get("temperature")) + "\thumidity: " + str(request.args.get("humidity"))