from lazarus_client import LazarusClient, LazarusClientError

def main():
    client = LazarusClient(port=8080)

    try:
        # Create container
        print("Creating container 'users'...")
        resp = client.create_container("users")
        print("Response:", resp)

        # Insert object
        print("Inserting object...")
        resp = client.insert_object("users", "user123", '{"name": "Alice", "age": 30}')
        print("Response:", resp)

        # Get object
        print("Retrieving object...")
        resp = client.get_object("users", "user123")
        print("Response:", resp)

        # Delete object
        print("Deleting object...")
        resp = client.delete_object("users", "user123")
        print("Response:", resp)

        # Delete container
        print("Deleting container 'users'...")
        resp = client.delete_container("users")
        print("Response:", resp)

    except LazarusClientError as e:
        print("Client error:", e)


if __name__ == "__main__":
    main()
