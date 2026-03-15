# ☄️ PandoraDB

PandoraDB is a high performance object container data store HTTP server implemented in C++ for GNU/Linux. 
It’s designed for **speed, persistence, and simplicity**, built on top of RocksDB.

---

## License

This software is licensed under the **Business Source License 1.1 (BSL)**.

You may use, modify, and distribute this software for non-commercial and non-production use only.  
Commercial or production use requires a license from the Licensor.

Contact: [pandoradatastore@gmail.com](mailto:pandoradatastore@gmail.com)

## 🧩 What is PandoraDB?

PandoraDB is an **object container store**:

- 📦 **Containers** act like logical buckets.
- 🔑 Each container holds **objects** stored as `key → value` pairs (IDs → byte streams).
- 🌐 All operations are exposed via a lightweight **HTTP API**.
- ⚡ Backed by RocksDB for persistence and performance.

---

## 🏗️ Architecture

PandoraDB is split into three conceptual layers:

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
- `PUT    /pandora/network/container_endpoint/` → create container
- `DELETE /pandora/network/container_endpoint/` → delete container

**Objects**
- `POST   /pandora/network/object_endpoint/` → insert object
- `GET    /pandora/network/object_endpoint/` → fetch object
- `DELETE /pandora/network/object_endpoint/` → remove object

---

## ⚡ Python SDK

A Python client lives in [`sdk/python`](./sdk/python).

Quick example:

```python
from pandora_client import PandoraDBClient

client = PandoraDBClient(port=8080)

client.create_container("users")
client.insert_object("users", "user123", '{"name": "Alice"}')
print(client.get_object("users", "user123"))
client.delete_object("users", "user123")
client.delete_container("users")
