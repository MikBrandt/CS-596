from picamera2 import Picamera2
camera = Picamera2()

camera.start()
camera.capture_file("test_image.jpg")
camera.close()

import boto3
import requests

BUCKET_NAME = 'cs596project'
REGION_NAME = 'us-west-2'
ACCESS_KEY = 'my_access_key'
SECRET_KEY = 'my_secret_key'

s3 = boto3.client('s3', region_name=REGION_NAME,
                  aws_access_key_id=ACCESS_KEY,
                  aws_secret_access_key=SECRET_KEY)

img_filepath = 'test_image.jpg'
s3.upload_file(img_filepath, BUCKET_NAME, 'test_image.jpg') # name should be changed so its unique
url = "http://34.209.250.108:5000"
requests.get(url) # in the future, this should include addresses that the server can use to send the picture to
