from flask import Flask
from flask import request
import smtplib
import boto3
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.mime.image import MIMEImage
from email.mime.base import MIMEBase
from email import encoders
app = Flask(__name__)

@app.route("/")
def hello():
        access_key = 'AKIAQHJN7BRKATNLSBWT'
        secret_key = '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'
        region = 'us-west-2'
        bucket_name = 'cs596project'
        object_key = 'test_image.jpg' #should be provided by pi

        s3 = boto3.resource('s3', aws_access_key_id=access_key, aws_secret_access_key=secret_key, region_name=region)
        s3_object = s3.Object(bucket_name, object_key)
        s3_object_body = s3_object.get()['Body'].read()
        email = 'noreplycs596project@gmail.com'
        password = 'vvrjwygvwewhavgz'
        phone_number = '6508924454' #should be provided by pi
        carrier_gateway = 'txt.att.net' #should be provided by pi
        message = 'Motion Detected!'
        msg = MIMEMultipart('mixed')
        msg_txt = MIMEMultipart()
        msg['From'] = email
        msg_txt['From'] = email
        msg_txt['To'] = phone_number+'@'+carrier_gateway
        msg['To'] = "mbrandt409@gmail.com" #should be provided by pi
        msg['Subject'] = message
        msg_txt['Subject'] = message
        msg.attach(MIMEText(message))
        image = MIMEImage(s3_object_body)
        msg.attach(image)
        smtp_server = smtplib.SMTP('smtp.gmail.com', 587)
        smtp_server.starttls()
        smtp_server.login(email, password)
        sms_gateway_txt = phone_number + '@' + carrier_gateway
        sms_gateway = "mbrandt409@gmail.com" #should be provided by pi
        smtp_server.sendmail(email, sms_gateway, msg.as_string())
        message_txt = "Motion Detected! View image: https://cs596project.s3.us-west-2.amazonaws.com/test_image.jpg" #should use image name provided by pi
        smtp_server.sendmail(email, sms_gateway_txt, msg_txt.as_string())
        smtp_server.quit()

        return 'Motion Detected'