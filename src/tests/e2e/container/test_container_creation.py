# pytest -v -s --tb=line -p no:warnings

from lazarus_client import LazarusClientError, LazarusStatusCode

def test_container_lifecycle(client):
    container_name = "test_container"
    object_id = "obj1"
    data_value = "hello world"

    # Create container
    client.create_container(container_name)

    try:
        client.create_container(container_name)
    except LazarusClientError as e:
        assert e.lazarus_status_code == LazarusStatusCode.container_already_exists

    # Insert object
    client.insert_object(container_name, object_id, data_value)

    # Retrieve
    data = client.get_object(container_name, object_id)
    assert data == data_value

    # Delete object
    client.remove_object(container_name, object_id)

    # Delete container
    client.remove_container(container_name)

    try:
        client.create_container(container_name)
    except LazarusClientError as e:
        assert e.lazarus_status_code == LazarusStatusCode.container_in_deletion_process
