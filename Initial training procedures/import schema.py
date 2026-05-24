import sqlite3

conn = sqlite3.connect('marklex.db')

with open('schema.sql', 'w') as f:
    for line in conn.iterdump():

        if 'CREATE TABLE' in line:
            f.write(f"{line}\n")

conn.close()
print("schema.sql is done!")