import sqlite3
import re
#https://www.kaggle.com/datasets/projjal1/human-conversation-training-data
#https://www.kaggle.com/datasets/kreeshrajani/3k-conversations-dataset-for-chatbot
DB_FILE = "marklex.db"
INPUT_FILE = "trainer_text_4.txt"

# DATABASE CONNECTION

conn = sqlite3.connect(DB_FILE)
cursor = conn.cursor()

# Enable foreign keys
cursor.execute("PRAGMA foreign_keys = ON")

# LOAD TRAINING TEXT

with open(INPUT_FILE, "r", encoding="utf-8") as file:
    text = file.read()

# PREPROCESSING

# convert to lowercase
text = text.lower()

# split text into sentences
sentences = re.split(r'[.!?]+', text)

# TRAINING LOOP

for sentence in sentences:

    # remove remaining punctuation
    sentence = re.sub(r"[^a-zA-Z'\s]", " ", sentence)

    # tokenize words
    words = sentence.split()

    # skip empty sentence
    if len(words) == 0:
        continue

    # contextual anchor
    prev_word_id = None

    # WORD LOOP

    for word in words:

        # INSERT WORD IF NOT EXISTS

        try:

            cursor.execute(
                """
                INSERT INTO word_list(word)
                VALUES(?)
                """,
                (word,)
            )

        except sqlite3.IntegrityError:
            # word already exists
            pass

        # GET CURRENT WORD ID

        cursor.execute(
            """
            SELECT id
            FROM word_list
            WHERE word = ?
            """,
            (word,)
        )

        curr_word_id = cursor.fetchone()[0]

        # FIRST WORD OF SENTENCE

        if prev_word_id is None:

            prev_word_id = curr_word_id
            continue

        # CHECK IF TRANSITION EXISTS

        cursor.execute(
            """
            SELECT frequency
            FROM frequency_list
            WHERE prev_word_id = ?
            AND curr_word_id = ?
            """,
            (prev_word_id, curr_word_id)
        )

        result = cursor.fetchone()

        # UPDATE EXISTING TRANSITION

        if result:

            cursor.execute(
                """
                UPDATE frequency_list
                SET frequency = frequency + 1
                WHERE prev_word_id = ?
                AND curr_word_id = ?
                """,
                (prev_word_id, curr_word_id)
            )

        # INSERT NEW TRANSITION

        else:

            cursor.execute(
                """
                INSERT INTO frequency_list
                (prev_word_id, curr_word_id, frequency)
                VALUES (?, ?, 1)
                """,
                (prev_word_id, curr_word_id)
            )

        # CHECK PARASITE WORD

        cursor.execute(
            """
            SELECT 1
            FROM parasite_list
            WHERE word_id = ?
            """,
            (curr_word_id,)
        )

        parasite = cursor.fetchone()


        # UPDATE CONTEXTUAL ANCHOR


        if not parasite:
            prev_word_id = curr_word_id

# SAVE DATABASE

conn.commit()

# CLOSE DATABASE

conn.close()
print("Training completed successfully.")