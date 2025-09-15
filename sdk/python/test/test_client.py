import pytest
from lazarus_client import LazarusClient, LazarusClientError

@pytest.fixture(scope="module")
def client():
    # Create one client for all tests
    return LazarusClient(port=8080)


def test_create_container(client):
    resp = client.create_container("users")
    assert resp is not None


def test_insert_object(client):
    resp = client.insert_object("users", "user123", '{"name": "Alice", "age": 30}')
    assert resp is not None


def test_get_object(client):
    resp = client.get_object("users", "user123")
    assert resp is not None
    assert "Alice" in str(resp)


def test_delete_object(client):
    resp = client.delete_object("users", "user123")
    assert resp is not None


def test_delete_container(client):
    resp = client.delete_container("users")
    assert resp is not None
