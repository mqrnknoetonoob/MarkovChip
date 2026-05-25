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

# Database Training Rules

## 1. Vocabulary Registration

All unique words are stored in the `word_list` table.

- If a word does not already exist:
  - it is inserted into the database.
- If the word already exists:
  - insertion is ignored due to the `UNIQUE` constraint.

---

## 2. Transition Frequency Tracking

The `frequency_list` table stores contextual word transitions.

Each row represents:

previous_word -> current_word

along with the frequency of occurrence.

---

## 3. Frequency Update Logic

During training:

- If a transition already exists:
  - its frequency is incremented.
- Otherwise:
  - a new transition entry is created with initial frequency `1`.

---

## 4. Parasite Word Mechanism

Function words such as:
- auxiliary verbs,
- prepositions,
- conjunctions,
- articles,

are stored in the `parasite_list` table.

These words are treated as contextual bridge words rather than semantic anchors.

---

## 5. Semantic Anchor Preservation

If the current word is identified as a parasite word:

- the transition is still recorded,
- BUT the contextual anchor (`prev_word`) is not updated.

This preserves long-range semantic context.

### Example

Input sentence:

he is very tired

If:

is

is marked as a parasite word,

the generated transitions become:

he -> is  
he -> very  
very -> tired

instead of:

is -> very

---

## 6. Sentence Boundary Handling

Training is performed sentence-by-sentence.

Sentence delimiters:
- `.`
- `!`
- `?`

are used to prevent invalid cross-sentence transitions.

Example:

He is tired. She is happy.

does NOT generate:

tired -> she

---

## 7. Text Normalization

Before training:

- all text is converted to lowercase,
- unnecessary punctuation is removed,
- apostrophes are preserved to maintain contractions such as:
  - don't
  - i'm
  - he's

---

## 8. Architecture Goal

The database is designed to generate a sparse probabilistic transition graph suitable for lightweight software inference and future hardware acceleration.
