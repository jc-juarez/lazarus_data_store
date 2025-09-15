# ☄️ Lazarus Data Store

Lazarus Data Store is a high performance object container data store HTTP server implemented in C++ for GNU/Linux. 
It’s designed for **speed, persistence, and simplicity**, built on top of RocksDB.

---

## 🧩 What is Lazarus DS?

Lazarus DS is an **object container store**:

- 📦 **Containers** act like logical buckets.
- 🔑 Each container holds **objects** stored as `key → value` pairs (IDs → byte streams).
- 🌐 All operations are exposed via a lightweight **HTTP API**.
- ⚡ Backed by RocksDB for persistence and performance.

---

## 🏗️ Architecture

Lazarus DS is split into three conceptual layers:

### 🌐 HTTP Layer
- Provides REST-style endpoints for containers and objects.
- Responsible for request handling and translating HTTP into storage operations.

### ⚙️ Storage Engine
- Manages containers (create, drop, list) and objects (insert, get, delete).
- Maps containers directly to RocksDB **column families**.
- Ensures error handling and logging are consistent.

### 💾 Persistence
- RocksDB handles the heavy lifting: WAL, SST files, compaction, caching.
- Tuned for parallelism and predictable performance under load.

---

## 🔑 Features

- 🗂️ **Multiple containers** via RocksDB column families.
- 📝 **Object CRUD** (create, retrieve, update, delete) via simple endpoints.
- 🚀 **Performance-tuned** RocksDB configs (parallelism, block cache).
- 📊 Clear **status codes and logging** on every operation.
- 🐧 Built for Linux servers.

---

## 🌐 HTTP API Overview

**Containers**
- `PUT    /lazarus/network/container_endpoint/` → create container
- `DELETE /lazarus/network/container_endpoint/` → delete container

**Objects**
- `POST   /lazarus/network/object_endpoint/` → insert object
- `GET    /lazarus/network/object_endpoint/` → fetch object
- `DELETE /lazarus/network/object_endpoint/` → remove object

---

## ⚡ Python SDK

A Python client lives in [`sdk/python`](./sdk/python).

Quick example:

```python
from lazarus_client import LazarusClient

client = LazarusClient(port=8080)

client.create_container("users")
client.insert_object("users", "user123", '{"name": "Alice"}')
print(client.get_object("users", "user123"))
client.delete_object("users", "user123")
client.delete_container("users")
