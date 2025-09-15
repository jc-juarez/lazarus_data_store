import requests
from requests.adapters import HTTPAdapter, Retry
from typing import Any, Dict, Optional

class LazarusClientError(Exception):
    """Base exception for Lazarus sdk errors."""
    pass


class LazarusClient:
    """
    Python sdk for Lazarus Data Store.
    Provides CRUD operations on containers and objects via HTTP.
    """

    def __init__(self, port: int, host: str = "http://localhost", timeout: int = 10):
        """
        Initialize the sdk.

        Args:
            port (int): Port where Lazarus server is running.
            host (str): Host address (default: "http://localhost").
            timeout (int): Timeout for requests in seconds (default: 10).
        """
        self.base_url = f"{host}:{port}/lazarus/network"
        self.timeout = timeout

        # Configure session with retries
        self.session = requests.Session()
        retries = Retry(
            total=5,
            backoff_factor=0.3,
            status_forcelist=(500, 502, 503, 504),
            allowed_methods=["GET", "POST", "DELETE"],
        )
        adapter = HTTPAdapter(max_retries=retries)
        self.session.mount("http://", adapter)
        self.session.mount("https://", adapter)

    def _request(self, method: str, endpoint: str, **kwargs) -> Any:
        """
        Internal helper to send HTTP requests and handle errors.
        """
        url = f"{self.base_url}/{endpoint}"
        try:
            response = self.session.request(method, url, timeout=self.timeout, **kwargs)
            response.raise_for_status()
            if response.headers.get("Content-Type", "").startswith("application/json"):
                return response.json()
            return response.text
        except requests.exceptions.RequestException as e:
            raise LazarusClientError(f"Request to {url} failed: {e}") from e

    # -------------------
    # Container operations
    # -------------------
    def create_container(self, container_name: str) -> Dict[str, Any]:
        """
        Create a new container.
        """
        payload = {"container_name": container_name}
        return self._request("PUT", "container_endpoint/", json=payload)

    def delete_container(self, container_name: str) -> Dict[str, Any]:
        """
        Delete an existing container.
        """
        payload = {"container_name": container_name}
        return self._request("DELETE", "container_endpoint/", json=payload)

    # ----------------
    # Object operations
    # ----------------
    def insert_object(self, container_name: str, object_id: str, data: str) -> Dict[str, Any]:
        """
        Insert an object into a container.
        """
        payload = {
            "container_name": container_name,
            "object_id": object_id,
            "object_data": data,
        }
        return self._request("POST", "object_endpoint/", json=payload)

    def get_object(self, container_name: str, object_id: str) -> Optional[str]:
        """
        Retrieve an object from a container.
        """
        payload = {
            "container_name": container_name,
            "object_id": object_id
        }
        return self._request("GET", "object_endpoint/", json=payload)

    def delete_object(self, container_name: str, object_id: str) -> Dict[str, Any]:
        """
        Delete an object from a container.
        """
        payload = {
            "container_name": container_name,
            "object_id": object_id
        }
        return self._request("DELETE", "object_endpoint/", json=payload)
