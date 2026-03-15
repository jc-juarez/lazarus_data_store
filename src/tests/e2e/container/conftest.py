import time
import pytest
from testcontainers.core.container import DockerContainer
from pandora_client import PandoraDBClient, PandoraDBClientError

IMAGE = "runtime-ubuntu-pandora"  # your image name
PORT = 8080


@pytest.fixture(scope="session")
def pandora_container():
    """Spin up the PandoraDB container for integration tests."""
    container = DockerContainer(IMAGE).with_exposed_ports(PORT)
    container.start()

    host = container.get_container_host_ip()
    mapped_port = int(container.get_exposed_port(PORT))

    # Wait for PandoraDB to become ready using the SDK itself
    client = PandoraDBClient(host=host, port=mapped_port)
    for _ in range(5):
        try:
            if client.ping():
                break
        except PandoraDBClientError:
            pass
        time.sleep(1)
    else:
        pytest.fail("PandoraDB container did not respond to ping within 30s")

    yield host, mapped_port
    container.stop()


@pytest.fixture(scope="session")
def client(pandora_container):
    """Provide a PandoraDBClient connected to the running container."""
    host, port = pandora_container
    return PandoraDBClient(host=host, port=int(port))
