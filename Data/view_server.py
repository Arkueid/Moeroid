from flask import Flask, render_template, request
import sqlite3
import os
from datetime import datetime


cd = os.path.split(__file__)[0]
template_folder = os.path.join(cd, "templates")


app = Flask(__name__,
            root_path=cd,
            static_folder="Audio"
            )


@app.route("/", methods=["GET"])
def index():
    day = request.args.get('date')
    if not day:
        day = datetime.now().strftime("%Y-%m-%d")
    chat_db = sqlite3.connect(os.path.join(cd, "History.db"))
    cursor = chat_db.execute(f"select * from messages where date(ct) = ? order by ct asc", (day, ))  
    res = cursor.fetchall()
    cursor.close()
    chat_db.close()
    return render_template("index.html", items=res, current_date=day)
    

if __name__ == "__main__":
    app.run("::", port=5090)