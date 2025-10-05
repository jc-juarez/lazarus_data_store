import time
import pytest
from testcontainers.core.container import DockerContainer
from lazarus_client import LazarusClient, LazarusClientError

IMAGE = "runtime-ubuntu-lazarus"  # your image name
PORT = 8080


@pytest.fixture(scope="session")
def lazarus_container():
    """Spin up the Lazarus data store container for integration tests."""
    container = DockerContainer(IMAGE).with_exposed_ports(PORT)
    container.start()

    host = container.get_container_host_ip()
    mapped_port = int(container.get_exposed_port(PORT))

    # Wait for Lazarus to become ready using the SDK itself
    client = LazarusClient(host=host, port=mapped_port)
    for _ in range(30):
        try:
            if client.ping():
                break
        except LazarusClientError:
            pass
        time.sleep(1)
    else:
        pytest.fail("Lazarus container did not respond to ping within 30s")

    yield host, mapped_port
    container.stop()


@pytest.fixture(scope="session")
def client(lazarus_container):
    """Provide a LazarusClient connected to the running container."""
    host, port = lazarus_container
    return LazarusClient(host=host, port=int(port))
