import sqlite3
import os

# File ebong Database er naam
TXT_FILE = "parasite_word.txt"  # Aponar txt file er naam ekhane din
DB_FILE = "marklex.db"

def insert_parasite_words():
    if not os.path.exists(TXT_FILE):
        print(f"Error: '{TXT_FILE}' file-ti khunje paoa zayni!")
        return

    conn = sqlite3.connect(DB_FILE)
    cursor = conn.cursor()

    print("Connection established.")
    
    try:
        with open(TXT_FILE, 'r', encoding='utf-8') as file:
            for line in file:
                # Line er pasher khali jaiga ba newline character clean korchi
                word = line.strip()
                
                # Jodi line-ti khali na hoy
                if word:
                    # ১. word_list table-e word-ti insert korchi (thakle ignore korbe UNIQUE er jonno)
                    cursor.execute("""
                        INSERT OR IGNORE INTO word_list (word) 
                        VALUES (?)
                    """, (word,))
                    
                    # ২. Word-er ID ti database theke tule anchhi
                    cursor.execute("""
                        SELECT id FROM word_list WHERE word = ?
                    """, (word,))
                    word_id = cursor.fetchone()[0]
                    
                    # ৩. parasite_list table-e shei ID-ti insert korchi
                    cursor.execute("""
                        INSERT OR IGNORE INTO parasite_list (word_id) 
                        VALUES (?)
                    """, (word_id,))

        conn.commit()
        print("Success!")
        
    except sqlite3.Error as e:
        print(f"Database Error: {e}")
        conn.rollback()
        
    finally:
        conn.close()

if __name__ == "__main__":
    insert_parasite_words()