from flask import Flask
from flask import request
import smtplib

app=Flask(__name__)
@app.route("/")
def hello():
    # Email address and password of the sender's email account
    email = 'noreplycs596project@gmail.com'
    password = 'vvrjwygvwewhavgz'

    # Recipient's phone number and carrier email-to-SMS gateway
    phone_number = '2342961990'
    carrier_gateway = 'txt.att.net'

    # Compose the message
    message = 'Motion Detected!'

    # Connect to the SMTP server of the sender's email provider
    smtp_server = smtplib.SMTP('smtp.gmail.com', 587)
    smtp_server.starttls()
    smtp_server.login(email, password)

    # Send the message as an email to the recipient's email-to-SMS gateway
    sms_gateway = phone_number + '@' + carrier_gateway
    smtp_server.sendmail(email, sms_gateway, message)

    # Close the SMTP server connection
    smtp_server.quit()
    print('Motion Detected!')
    return 'Motion Detected!'