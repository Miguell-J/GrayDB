<div align="center">
  
![GitHub repo size](https://img.shields.io/github/repo-size/Miguell-J/GrayDB?style=for-the-badge)
![GitHub language count](https://img.shields.io/github/languages/count/Miguell-J/GrayDB?style=for-the-badge)
![GitHub forks](https://img.shields.io/github/forks/Miguell-J/GrayDB?style=for-the-badge)
![Bitbucket open issues](https://img.shields.io/bitbucket/issues/Miguell-J/GrayDB?style=for-the-badge)
![Bitbucket open pull requests](https://img.shields.io/bitbucket/pr-raw/Miguell-J/GrayDB?style=for-the-badge)

  
# GrayDB
</div>

<div align="center">
  
![](/gimage.png)

</div>

> ⚙️ Lightweight, embeddable, modular database engine written in pure C — built from scratch, with B+ Trees, hash tables, and a custom query DSL.

---

## 🚀 Overview

**GrayDB** is a minimal, high-performance embedded database engine written entirely in C. Inspired by the architectural simplicity of SQLite and the modular power of PostgreSQL internals, it uses its own memory-efficient **B+ Tree** index, open-addressing **Hash Table**, and a small, expressive **domain-specific language (DSL)** for querying.

GrayDB is designed for learning, extensibility, and experimentation, without sacrificing performance or elegance.

---

## 🔧 Features

- 📁 **B+ Tree Indexing**  
  Fast insertion, deletion, and range queries with balanced tree structure.

- 🧠 **Open Hash Table**  
  Used internally for symbol resolution and future metadata support.

- 🧵 **Multithreading-ready**  
  Uses `pthread` and `semaphores` for concurrency (planned: lock-free optimizations).

- 🗃️ **Table Abstraction**  
  Tables support dynamically-typed columns and are represented as nodes in the index tree.

- 💬 **Custom DSL Parser**  
  Lightweight SQL-inspired syntax: `create table`, `insert into`, `select * from`, etc.

- 🔣 **REPL CLI Interface**  
  With `readline` integration for history and interactive queries.

---

## 🧱 Architecture

```
src/             # Source code
└── main.c       # Entry point
└── cli.c        # Interface & REPL
└── dsl.c        # DSL parser and dispatch
└── table.c      # Logical tables and schema
└── ds.c         # Core data structures (B+ tree, hash table)
└── crypt.c      # (Planned) data encryption layer
include/
├── \*.h         # All headers and interfaces

````

- 🌳 **B+ Tree**: Manages index nodes and supports efficient lookup, split, and rebalance operations.
- 🔐 **Hash Table**: Used for internal mappings and planned query caching.
- 🗂️ **Table Layer**: Maps columnar data to key-value entries in the B+ Tree.

---

## 🛠️ Usage

### Build

```bash
gcc -o graydb src/main.c src/cli.c src/dsl.c src/ds.c src/table.c -Iinclude -lreadline
````

### Run

```bash
./graydb
```

---

## 💻 Example

```
  .g8'''bgd  `7MM'''Mq.        db      `YMM'   `MM'`7MM'''Yb.   `7MM'''Yp,
.dP'     `M    MM   `MM.      ;MM:       VMA   ,V    MM    `Yb.   MM    Yb
dM'       `    MM   ,M9      ,V^MM.       VMA ,V     MM     `Mb   MM    dP
MM             MMmmdM9      ,M  `MM        VMMP      MM      MM   MM'''bg.
MM.    `7MMF'  MM  YM.      AbmmmqMA        MM       MM     ,MP   MM    `Y
`Mb.     MM    MM   `Mb.   A'     VML       MM       MM    ,dP'   MM    ,9
  `'bmmmdPY  .JMML. .JMM..AMA.   .AMMA.   .JMML.   .JMMmmmdP'   .JMMmmmd9

-----------------------------------------------------------------------------

Welcome to GrayDB:
>> create table users
>> insert into users values (1, "Miguel", 19)
>> select * from users where id = 1
>> delete from users where id = 1
>> exit
```

---

## 📚 DSL Commands (in progress)

| Command                | Description                          |
| ---------------------- | ------------------------------------ |
| `create table <name>`  | Creates a new table                  |
| `insert into <name>`   | Inserts a new row                    |
| `select * from <name>` | Fetches all data (soon with filters) |
| `delete from <name>`   | Deletes a row by primary key         |
| `clear`                | Clears the terminal                  |
| `exit`                 | Exits the REPL                       |

---

## 🎯 Roadmap

* [x] B+ Tree with split and rebalance
* [x] Hash table with open addressing
* [x] Table schema and metadata
* [x] DSL parser with REPL
* [ ] DSL: typed columns in `CREATE`
* [ ] `INSERT` with real values
* [ ] File-based persistence
* [ ] Query optimization engine
* [ ] Built-in cryptography with AES
* [ ] FOX (comming soon procedural language)

---

## 🧠 Author

**Miguel Araújo Julio**
Computer Science student

---

## 📜 License

MIT License. Free for use, study, and modification.

---

## 🤝 Contribute

GrayDB is a solo project, but open to collaboration.
If you're into compilers, OS dev, or low-level systems — feel free to fork and explore!
