# pytest -v --tb=line -p no:warnings

def test_container_lifecycle(client):
    container_name = "test_container"
    object_id = "obj1"
    data_value = "hello world"

    # Create container
    client.create_container(container_name)

    # Insert object
    client.insert_object(container_name, object_id, data_value)

    # Retrieve
    data = client.get_object(container_name, object_id)
    assert data == data_value

    # Delete object
    client.delete_object(container_name, object_id)

    # Delete container
    client.delete_container(container_name)
