# MarkLex — Initial Training Procedures

MarkLex is a lightweight predictive text system based on sparse probabilistic Markov-chain inference.

The project is being developed as a hardware-oriented NLP architecture targeting efficient future ASIC implementation for smartphone-style text prediction systems.

---

# Core Idea

Instead of treating every word equally, MarkLex preserves semantic anchors while selectively bypassing low-semantic grammatical bridge words (called parasite words).

This creates a sparse contextual transition graph suitable for:
- lightweight inference,
- reduced memory complexity,
- sparse lookup acceleration,
- future hardware implementation.

---

# Current Architecture

The current software prototype consists of:

- SQLite-based sparse transition database
- Corpus training pipeline
- Frequency-based contextual transition generation
- Parasite-word contextual anchoring

---

# Project Structure

```text
Initial training procedures/
│
├── marklex.db
├── train.py
├── parasite_word_inserter.py
├── parasite_words.txt
├── training_text.txt
└── README.md
```

Database Schema

The system currently uses three tables:

## Database Schema

The system currently uses three tables to manage and process text transitions efficiently:

| Table Name | Purpose |
| :--- | :--- |
| **`word_list`** | Stores all unique words. |
| **`frequency_list`** | Stores contextual transition frequencies. |
| **`parasite_list`** | Stores low-semantic grammatical bridge words. |

## **Database Setup**

Create the database schema before training.

Example:

```bash
sqlite3 marklex.db
```

## **Populate Parasite Words**

To insert the required words from the parasite-word list into the database, run:

```bash
python parasite_word_inserter.py
```

## **Run Training**

To train the transition database using the provided corpus:

```bash
python main_trainer_script.py
```

## **Database Training Rules**

### **1. Vocabulary Registration**
All unique words are stored in the `word_list` table.

* **If a word does not already exist:** It is inserted into the database.
* **If the word already exists:** Insertion is ignored due to the `UNIQUE` constraint.

### **2. Transition Frequency Tracking**
The `frequency_list` table stores contextual word transitions.

Each row represents:
```bash
previous_word -> current_word
```

### **3. Frequency Update Logic**
During training:

* **If a transition already exists:** Its frequency is incremented.
* **Otherwise:** A new transition entry is created with an initial frequency of 1.

---

### **4. Parasite Word Mechanism**
Function words such as:
* Articles
* Auxiliary verbs
* Conjunctions
* Prepositions

Are stored in the `parasite_list` table. These words are treated as contextual bridge words rather than semantic anchors.

### **5. Semantic Anchor Preservation**
If the current word is identified as a parasite word:
* The transition is still recorded,
* **BUT** the contextual anchor (`prev_word`) is not updated.

This preserves longer semantic context across grammatical bridge words.

#### **Example**
Input sentence:
```bash
he is very tired
```
"is" is marked as a parasite word, so the generated transitions become:
```bash
he -> is
he -> very
very -> tired
```
instead of
```bash
is -> very
```

### **6. Text Normalization**
Before training:
* All text is converted to lowercase.
* Unnecessary punctuation is removed.
* Apostrophes are preserved to maintain contractions such as:
```bash
  don't
  i'm
  he's
```
* ".", ",", "!", "?" is used as sentence delimeter 

## **Current Goal**

The current objective is to build a **sparse probabilistic transition graph** suitable for future lightweight hardware inference.

### **Long-Term Target Includes:**
* **Top-K Prediction Selection:** Optimizing next-token selection efficiency.
* **Sparse Lookup Acceleration:** Reducing memory access overhead for hardware.
* **RTL Implementation:** Translating the transition logic into hardware description languages.
* **ASIC-Oriented Predictive Text Inference:** Designing dedicated silicon-level architectures for low-power predictive text.
