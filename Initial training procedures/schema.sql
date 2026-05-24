CREATE TABLE frequency_list (
    prev_word_id INTEGER,
    curr_word_id INTEGER,
    frequency INTEGER DEFAULT 0,

    PRIMARY KEY(prev_word_id, curr_word_id),

    FOREIGN KEY(prev_word_id) REFERENCES word_list(id),
    FOREIGN KEY(curr_word_id) REFERENCES word_list(id)
);
CREATE TABLE parasite_list (
    word_id INTEGER,

    PRIMARY KEY(word_id),

    FOREIGN KEY(word_id) REFERENCES word_list(id)
);
CREATE TABLE word_list (
    id INTEGER PRIMARY KEY AUTOINCREMENT, 
    word TEXT UNIQUE
);
