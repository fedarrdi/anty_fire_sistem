import smtplib, ssl

def send_mail(message):
    port = 465  # For SSL
    smtp_server = "smtp.gmail.com"
    sender_email = "meinmax82@gmail.com"  # Enter your address
    receiver_email = "fedarradi@gmail.com"  # Enter receiver address
    password = ""
    context = ssl.create_default_context()
    with smtplib.SMTP_SSL(smtp_server, port, context=context) as server:
        server.login(sender_email, password)
        server.sendmail(sender_email, receiver_email, message)

