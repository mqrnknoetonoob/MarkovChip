# Initial Training Procedures

This folder contains all the necessary scripts, data sources, and SQL files used for the initial model training and database preparation of the **MarkLex** project.

---

## 📂 Folder Structure & File Guide

Here is a quick overview of the files included in this directory and their respective purposes:

* **`main_trainer_script.py`**: The primary training script used to train the model.
* **`parasite_word_inserter.py`**: A utility script designed to process and insert parasite words into the database.
* **`marklex.db`**: The local SQLite database file containing the training data.
* **`schema.sql`**: The database schema file containing the structure (`CREATE TABLE` statements) of the database.
* **`testquery.sql`**: A collection of SQL queries used for testing and verifying database records.
* **`parasite_word.txt`**: A raw text file containing the list of parasite words to be processed.
* **`trainer_text_1.txt` to `trainer_text_4.txt`**: Raw text corpora used as data sources for model training.

---

## 🚀 How to Run

Before running the scripts, ensure you have Python 3.x installed on your system along with any required dependencies.

### 1. Database Setup
If you do not have the `marklex.db` file populated, you can recreate the database structure using the provided `schema.sql` file, or simply run the pipeline scripts below.

### 2. Populate Parasite Words
To insert the required words from the text list into the database, run:
```bash
python parasite_word_inserter.py
